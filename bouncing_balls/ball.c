#include "ball.h"

Ball *balls_create(void) {
    return malloc(sizeof(Ball) * ball_count);
}

void balls_destroy(Ball *balls) {
    free(balls);
}

void ball_render(const Ball *ball) {
    DrawCircleV(ball->pos, ball->radius, RAYWHITE);
}

void ball_update(Ball *ball) {
    ball->pos.x += ball->vel.x * GetFrameTime();
    ball->pos.y += ball->vel.y * GetFrameTime();

    if (ball->pos.x - ball->radius < 0) {
        ball->pos.x = ball->radius;
        ball->vel.x *= -1;
    } else if (ball->pos.x + ball->radius > width) {
        ball->pos.x = width - ball->radius;
        ball->vel.x *= -1;
    }
    if (ball->pos.y - ball->radius < 0) {
        ball->pos.y = ball->radius;
        ball->vel.y *= -1;
    } else if (ball->pos.y + ball->radius > height) {
        ball->pos.y = height - ball->radius;
        ball->vel.y *= -1;
    }
}

void ball_resolve_collision(Ball *a, Ball *b) {
    Vector2 impact_vector = Vector2Subtract(b->pos, a->pos);
    float dist = Vector2Length(impact_vector);
    if (dist < a->radius + b->radius) {
        // resolve collision
        float overlap = dist - (a->radius + b->radius);
        Vector2 dir = Vector2Scale(Vector2Normalize(impact_vector), overlap / 2);
        a->pos = Vector2Add(a->pos, dir);
        dir = Vector2Negate(dir);
        b->pos = Vector2Add(b->pos, dir);

        float masses = a->mass + b->mass;
        Vector2 vel_diff = Vector2Subtract(b->vel, a->vel);
        float den = masses * dist * dist;

        float num = (2 * b->mass) * Vector2DotProduct(vel_diff, impact_vector);
        Vector2 vdA = Vector2Scale(impact_vector, num / den);
        num *= -(a->mass / b->mass);
        Vector2 vdB = Vector2Scale(impact_vector, num / den);

        a->vel = Vector2Add(a->vel, vdA);
        b->vel = Vector2Add(b->vel, vdB);
    }
}
