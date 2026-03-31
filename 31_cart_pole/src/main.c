#include <math.h>
#include <random.h>
#include <raylib.h>
#include <string.h>

// ── Simulation ────────────────────────────────────────────────────────────────
#define WIDTH 1280
#define HEIGHT 720
#define FPS 60

#define P_W 100
#define P_H 50
#define P_W2 (P_W / 2.0)
#define P_H2 (P_H / 2.0)
#define P_MASS 10.0

#define Q_LEN 120
#define Q_THICK 10
#define Q_MASS 4.0

#define GRAVITY 1000.0
#define FORCE_MAG 10000.0
#define MAX_DEV 0.5f // radians from upright before episode resets

typedef struct {
  double p_pos, p_vel, p_acc;
  double a, a_vel, a_acc;
} State;

// ── Neural network ────────────────────────────────────────────────────────────
#define NN_IN 4
#define NN_H 64
#define NN_OUT 3 // 0 = left, 1 = nothing, 2 = right

typedef struct {
  float w1[NN_H][NN_IN], b1[NN_H];
  float w2[NN_OUT][NN_H], b2[NN_OUT];
  float h_pre[NN_H], h[NN_H], out[NN_OUT]; // cached for backprop
} Net;

static void net_init(Net *n) {
  // He initialisation — correct scale for ReLU
  float s1 = sqrtf(2.0f / NN_IN);
  float s2 = sqrtf(2.0f / NN_H);
  for (int i = 0; i < NN_H; i++) {
    n->b1[i] = 0;
    for (int j = 0; j < NN_IN; j++) n->w1[i][j] = rng_range(-1, 1) * s1;
  }
  for (int i = 0; i < NN_OUT; i++) {
    n->b2[i] = 0;
    for (int j = 0; j < NN_H; j++) n->w2[i][j] = rng_range(-1, 1) * s2;
  }
}

static void net_forward(Net *n, const float *in) {
  for (int i = 0; i < NN_H; i++) {
    float s = n->b1[i];
    for (int j = 0; j < NN_IN; j++) s += n->w1[i][j] * in[j];
    n->h_pre[i] = s;
    n->h[i] = s > 0 ? s : 0; // ReLU
  }
  for (int i = 0; i < NN_OUT; i++) {
    float s = n->b2[i];
    for (int j = 0; j < NN_H; j++) s += n->w2[i][j] * n->h[j];
    n->out[i] = s;
  }
}

// SGD on a single (state, action, target_q) tuple.
// Only the row of w2/b2 corresponding to `action` is updated.
static void net_train(Net *n, const float *in, int action, float target, float lr) {
  net_forward(n, in);
  float d_out = n->out[action] - target;

  // Compute d_h from w2 BEFORE updating w2, otherwise the gradient is wrong
  float d_h[NN_H];
  for (int j = 0; j < NN_H; j++)
    d_h[j] = d_out * n->w2[action][j];

  for (int j = 0; j < NN_H; j++)
    n->w2[action][j] -= lr * d_out * n->h[j];
  n->b2[action] -= lr * d_out;

  // Backprop through ReLU
  for (int i = 0; i < NN_H; i++) {
    if (n->h_pre[i] <= 0) continue;
    for (int j = 0; j < NN_IN; j++)
      n->w1[i][j] -= lr * d_h[i] * in[j];
    n->b1[i] -= lr * d_h[i];
  }
}

// ── Replay buffer ─────────────────────────────────────────────────────────────
#define BUF_CAP 10000
#define BATCH_SIZE 64
#define LR 1e-3f
#define GAMMA 0.99f
#define EPS_START 1.0f
#define EPS_END 0.05f
#define EPS_DECAY 5000    // steps until epsilon reaches EPS_END
#define TARGET_UPDATE 200 // steps between target net syncs

typedef struct {
  float s[NN_IN];
  int a;
  float r;
  float ns[NN_IN];
  int done;
} Exp;

typedef struct {
  Exp data[BUF_CAP];
  int head, size;
} ReplayBuf;

static void buf_push(ReplayBuf *b, const Exp *e) {
  b->data[b->head] = *e;
  b->head = (b->head + 1) % BUF_CAP;
  if (b->size < BUF_CAP) b->size++;
}

// ── DQN agent ─────────────────────────────────────────────────────────────────
typedef struct {
  Net online, target;
  ReplayBuf replay;
  float epsilon;
  int steps;
} Agent;

static void agent_init(Agent *ag) {
  net_init(&ag->online);
  ag->target = ag->online;
  memset(&ag->replay, 0, sizeof(ag->replay));
  ag->epsilon = EPS_START;
  ag->steps = 0;
}

static int agent_act(Agent *ag, const float *s) {
  ag->epsilon = EPS_END + (EPS_START - EPS_END) * expf(-(float)ag->steps / EPS_DECAY);
  if (rng() < ag->epsilon) return floor(rng_range(0, NN_OUT)); // return rand() % NN_OUT;
  net_forward(&ag->online, s);
  int best = 0;
  for (int i = 1; i < NN_OUT; i++)
    if (ag->online.out[i] > ag->online.out[best]) best = i;
  return best;
}

static void agent_learn(Agent *ag) {
  if (ag->replay.size < BATCH_SIZE) return;
  for (int b = 0; b < BATCH_SIZE; b++) {
    Exp *e = &ag->replay.data[(int)rng_range(0, ag->replay.size)];
    float tq;
    if (e->done) {
      tq = e->r;
    } else {
      net_forward(&ag->target, e->ns);
      float best = ag->target.out[0];
      for (int i = 1; i < NN_OUT; i++)
        if (ag->target.out[i] > best) best = ag->target.out[i];
      tq = e->r + GAMMA * best;
    }
    net_train(&ag->online, e->s, e->a, tq, LR);
  }
  ag->steps++;
  if (ag->steps % TARGET_UPDATE == 0)
    ag->target = ag->online;
}

// ── Helpers ───────────────────────────────────────────────────────────────────
// Returns angular deviation from upright (a = PI), wrapped to [0, PI]
static float angle_dev(double a) {
  float d = fmodf(fabsf((float)a), 2.0f * PI);
  return d > PI ? 2.0f * PI - d : d;
}

static void get_nn_state(const State *sim, float *s) {
  s[0] = (float)(sim->p_pos / WIDTH) * 2.0f - 1.0f; // cart position  [-1, 1]
  s[1] = (float)(sim->p_vel / 1000.0f);             // cart velocity  (approx)
  s[2] = angle_dev(sim->a) / PI;                    // pole deviation [0,  1]
  s[3] = (float)(sim->a_vel / 10.0f);               // angular vel    (approx)
}

static void reset(State *s) {
  s->p_pos = WIDTH / 2.0;
  s->p_vel = 0;
  s->p_acc = 0;
  s->a = (rng() * 0.1f - 0.05f); // small random nudge
  s->a_vel = 0;
  s->a_acc = 0;
}

// ── Physics (your code, unchanged) ───────────────────────────────────────────
static void update(State *s, Vector2 *q, double F, double dt) {
  const double friction = pow(0.997, dt * 240.0);
  double D = P_MASS + Q_MASS * sin(s->a) * sin(s->a);
  s->p_acc = (F + Q_MASS * Q_LEN * s->a_vel * s->a_vel * sin(s->a) - Q_MASS * GRAVITY * sin(s->a) * cos(s->a)) / D;
  s->a_acc = ((P_MASS + Q_MASS) * GRAVITY * sin(s->a) - F * cos(s->a) - Q_MASS * Q_LEN * s->a_vel * s->a_vel * sin(s->a) * cos(s->a)) / (Q_LEN * D);
  s->p_vel += s->p_acc * dt;
  s->p_vel *= friction;
  s->a_vel += s->a_acc * dt;
  s->p_pos += s->p_vel * dt;
  s->a += s->a_vel * dt;
  if (s->p_pos <= P_W2) {
    s->p_pos = P_W2;
    if (s->p_vel < 0) s->p_vel = 0;
  } else if (s->p_pos >= WIDTH - P_W2) {
    s->p_pos = WIDTH - P_W2;
    if (s->p_vel > 0) s->p_vel = 0;
  }
  q->x = s->p_pos + Q_LEN * sin(s->a);
  q->y = HEIGHT / 2.0 + Q_LEN * -cos(s->a);
}

// ── Main ──────────────────────────────────────────────────────────────────────
int main(void) {
  InitWindow(WIDTH, HEIGHT, "Cart Pole — DQN");
  SetTargetFPS(FPS);

  State sim;
  reset(&sim);
  Agent ag;
  agent_init(&ag);

  Vector2 p = {0}, q = {0};
  float cur[NN_IN], nxt[NN_IN];
  int episode = 0, steps_alive = 0, best_steps = 0;

  while (!WindowShouldClose()) {
    double dt = 1.0 / 240.0;

    for (int i = 0; i < 4; i++) {
      get_nn_state(&sim, cur);

      int action = agent_act(&ag, cur);
      double F = 0;
      if (action == 0) F = -FORCE_MAG;
      if (action == 2) F = FORCE_MAG;

      update(&sim, &q, F, dt);
      steps_alive++;

      int done = angle_dev(sim.a) > MAX_DEV;
      float reward = done ? -1.0f : 1.0f;

      get_nn_state(&sim, nxt);
      Exp e;
      memcpy(e.s, cur, sizeof(e.s));
      memcpy(e.ns, nxt, sizeof(e.ns));
      e.a = action;
      e.r = reward;
      e.done = done;
      buf_push(&ag.replay, &e);
      agent_learn(&ag);

      if (done) {
        if (steps_alive > best_steps) best_steps = steps_alive;
        episode++;
        steps_alive = 0;
        reset(&sim);
      }
    }
    p.x = sim.p_pos;
    p.y = HEIGHT / 2.0f;
    BeginDrawing();
    ClearBackground(BLACK);
    DrawLineEx((Vector2){0, HEIGHT / 2.0f}, (Vector2){WIDTH, HEIGHT / 2.0f}, 3, GetColor(0xFFFFFF40));
    DrawRectanglePro((Rectangle){sim.p_pos, HEIGHT / 2.0f, P_W, P_H}, (Vector2){P_W2, P_H2}, 0.0f, GetColor(0x4060A0FF));
    DrawLineEx(p, q, Q_THICK, GetColor(0xD0A080FF));
    DrawCircleV(p, Q_THICK * 0.5f, GetColor(0xD0A080FF));
    DrawCircleV(q, Q_THICK * 0.5f, GetColor(0xD0A080FF));
    DrawCircleV(p, Q_THICK * 0.4f, GetColor(0x808080FF));
    DrawText(TextFormat("Episode: %d  |  Steps: %d  |  Best: %d  |  Eps: %.3f  |  Buf: %d",
                        episode, steps_alive, best_steps, ag.epsilon, ag.replay.size),
             10, 10, 20, WHITE);
    EndDrawing();
  }

  CloseWindow();
  return 0;
}
