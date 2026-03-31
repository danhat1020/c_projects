#include <math.h>
#include <random.h>
#include <raylib.h>
#include <rlgl.h>
#include <stdlib.h>

#define WIDTH 1000
#define HEIGHT 1000
#define TITLE "Three Body Problem"
#define FPS 60

#define GRAVITY 1.0
#define DT 0.00225
#define SOFTENING 0.5
#define N_BODIES 3

#define COL_DAMP 4

typedef struct {
  double x, y;
} vec2;

typedef struct {
  vec2 last_pos;
  vec2 pos, vel, acc;
  vec2 acc_new;
  Color col;
} Body;

// ===  GLOBALS  ===
Body *bodies;

// === FUNCTIONS ===
static Vector2 to_screen(vec2 *p, double scale) {
  return (Vector2){
      (double)(WIDTH / 2.0 + p->x * scale),
      (double)(HEIGHT / 2.0 - p->y * scale),
  };
}

static vec2 compute_acc(Body *bodies, int i) {
  vec2 acc = {0, 0};
  for (int j = 0; j < N_BODIES; j++) {
    if (j == i) { continue; }
    double dx = bodies[j].pos.x - bodies[i].pos.x;
    double dy = bodies[j].pos.y - bodies[i].pos.y;
    double dist_sq = dx * dx + dy * dy + SOFTENING * SOFTENING;
    double dist = sqrt(dist_sq);
    double force = GRAVITY * 1 / dist_sq;
    acc.x += force * (dx / dist);
    acc.y += force * (dy / dist);
  }
  return acc;
}

static void step(Body *bodies) {
  // update positions
  for (int i = 0; i < N_BODIES; i++) {
    bodies[i].pos.x += bodies[i].vel.x * DT + 0.5 * bodies[i].acc.x * DT * DT;
    bodies[i].pos.y += bodies[i].vel.y * DT + 0.5 * bodies[i].acc.y * DT * DT;
  }
  // compute new acc
  for (int i = 0; i < N_BODIES; i++) {
    bodies[i].acc_new = compute_acc(bodies, i);
  }
  // update velocities using average of old + new acc
  for (int i = 0; i < N_BODIES; i++) {
    bodies[i].vel.x += 0.5 * (bodies[i].acc.x + bodies[i].acc_new.x) * DT;
    bodies[i].vel.y += 0.5 * (bodies[i].acc.y + bodies[i].acc_new.y) * DT;
    bodies[i].acc = bodies[i].acc_new;
  }
}

int main(void) {
  InitWindow(WIDTH, HEIGHT, TITLE);
  SetTargetFPS(FPS);

  int frameCount = 0;

  bodies = malloc(N_BODIES * sizeof(Body));

  vec2 average = {0, 0};

  double ap = PI * 2 / 3;
  double lenp = 0.6;
  for (int i = 0; i < N_BODIES; i++) {
    double api = ap * i;
    double av = (api) - (PI / 2.0);
    double lenv = 0.6 * rng_range(0.8, 1.25);
    vec2 pos = (vec2){lenp * cos(ap * i), lenp * sin(ap * i)};
    vec2 vel = (vec2){lenv * cos(av), lenv * sin(av)};

    if (i == 2) { vel.x = -average.x, vel.y = -average.y; }

    bodies[i] = (Body){
        .last_pos = pos,
        .pos = pos,
        .vel = vel,
    };
    average.x += vel.x;
    average.y += vel.y;
  }
  bodies[0].col = RED;
  bodies[1].col = SKYBLUE;
  bodies[2].col = GREEN;

  RenderTexture2D target = LoadRenderTexture(WIDTH, HEIGHT);
  double scale = 200.0;

  for (int i = 0; i < N_BODIES; i++) {
    bodies[i].acc = compute_acc(bodies, i);
  }

  int steps_per_frame = 20;
  int paused = 0;

  while (!WindowShouldClose()) {
    frameCount++;
    if (IsKeyPressed(KEY_SPACE)) { paused = 1 - paused; }
    // update state
    if (!paused) {
      for (int i = 0; i < N_BODIES; i++) {
        bodies[i].last_pos = bodies[i].pos;
      }
      for (int s = 0; s < steps_per_frame; s++) {
        step(bodies);
      }
    }

    BeginTextureMode(target);
    for (int i = 0; i < N_BODIES; i++) {
      Vector2 sp = to_screen(&bodies[i].pos, scale);
      Vector2 sp2 = to_screen(&bodies[i].last_pos, scale);
      Color col = Fade(ColorLerp(bodies[i].col, WHITE, 0.4), 0.8);
      DrawLineEx(sp, sp2, 2.0, col);
    }
    rlSetBlendFactors(RL_ONE, RL_ONE, RL_FUNC_REVERSE_SUBTRACT);
    rlSetBlendMode(BLEND_CUSTOM);
    DrawRectangle(0, 0, WIDTH, HEIGHT, (Color){COL_DAMP, COL_DAMP, COL_DAMP, 0});
    rlSetBlendMode(BLEND_ALPHA);
    EndTextureMode();

    BeginDrawing();
    ClearBackground(BLACK);

    DrawTexturePro(target.texture,
                   (Rectangle){0, 0, WIDTH, -HEIGHT},
                   (Rectangle){0, 0, WIDTH, HEIGHT},
                   (Vector2){0, 0},
                   0.0,
                   WHITE);

    BeginBlendMode(BLEND_ADDITIVE);
    double max_radius = 40.0;
    int steps = 20;
    for (int i = 0; i < N_BODIES; i++) {
      Vector2 sp = to_screen(&bodies[i].pos, scale);
      for (int n = 0; n < steps; n++) {
        double t = (double)n / steps;
        double radius = max_radius * t;
        double alpha = expf(-6.7 * powf(t, 0.8));
        Color col = Fade(bodies[i].col, alpha);
        DrawCircleV(sp, radius, col);
      }
    }
    EndBlendMode();

    EndDrawing();
  }

  UnloadRenderTexture(target);

  CloseWindow();
  return 0;
}
