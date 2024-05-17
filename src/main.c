#include "raylib.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define WIDTH 800
#define HEIGHT 800
#define MARGIN 20

#define DRAND_MIN -10
#define DRAND_MAX 10

#define STEP_SIZE 0.01

double randd(void) {
    double range = DRAND_MAX - DRAND_MIN;
    return DRAND_MIN + (rand() * range) / RAND_MAX;
}

typedef struct {
    // Data
    size_t N;
    double *xs;
    double *ys;
    Vector2 min;
    Vector2 max;

    // Weights
    double w;
    double b;
} Regression;

double mse_loss(Regression r) {
    double loss = 0.0f;
    for (size_t i = 0; i < r.N; ++i)
        loss += pow(r.w * r.xs[i] + r.b - r.ys[i], 2);

    return loss / r.N;
}

double d_mse_loss_w(Regression r) {
    double dloss = 0;
    for (size_t i = 0; i < r.N; ++i)
        dloss += r.xs[i] * (r.w * r.xs[i] + r.b - r.ys[i]);

    return 2.0f * dloss / r.N;
}

double d_mse_loss_b(Regression r) {
    double dloss = 0;
    for (size_t i = 0; i < r.N; ++i)
        dloss += r.w * r.xs[i] + r.b - r.ys[i];

    return 2.0f * dloss / r.N;
}

Regression regression_new(double *xs, double *ys, int N) {
    Regression r = {
        .N = N,
        .xs = xs,
        .ys = ys,
        .w = -randd(),
        .b = -randd(),
        .min = (Vector2){INFINITY, INFINITY},
        .max = (Vector2){-INFINITY, -INFINITY},
    };

    for (size_t i = 0; i < r.N; ++i) {
        double x = r.xs[i];
        double y = r.ys[i];

        if (x < r.min.x)
            r.min.x = x;
        if (y < r.min.y)
            r.min.y = y;

        if (x > r.max.x)
            r.max.x = x;
        if (y > r.max.y)
            r.max.y = y;
    }
    return r;
}

void regression_step(Regression *r) {
    r->w -= STEP_SIZE * d_mse_loss_w(*r);
    r->b -= STEP_SIZE * d_mse_loss_b(*r);
}

void draw_axis(void) {
    // X axis
    DrawLineEx((Vector2){MARGIN - 2, HEIGHT - MARGIN},
               (Vector2){WIDTH - MARGIN, HEIGHT - MARGIN}, 3.0, BLACK);
    Vector2 v1 = (Vector2){WIDTH - MARGIN + 2, HEIGHT - MARGIN};
    Vector2 v2 = (Vector2){v1.x - 8.66, v1.y + 5};
    Vector2 v3 = (Vector2){v1.x - 8.66, v1.y - 5};
    DrawText("x", v1.x - 10, v1.y - 20, 12, BLACK);
    DrawTriangle(v3, v2, v1, BLACK);

    // Y axis
    DrawLineEx((Vector2){MARGIN, MARGIN}, (Vector2){MARGIN, HEIGHT - MARGIN},
               3.0, BLACK);
    v1 = (Vector2){MARGIN, MARGIN - 2};
    v2 = (Vector2){v1.x - 5, v1.y + 8.66};
    v3 = (Vector2){v1.x + 5, v1.y + 8.66};
    DrawText("y", v1.x + 10, v1.y, 12, BLACK);
    DrawTriangle(v1, v2, v3, BLACK);
}

Vector2 get_point_pos(double x, double y, Regression r) {
    Vector2 pos = {x, y};
    pos.x = (pos.x - r.min.x) / (r.max.x - r.min.x);
    pos.x = MARGIN + (WIDTH - 2 * MARGIN) * pos.x;

    pos.y = (pos.y - r.min.y) / (r.max.y - r.min.y);
    pos.y = HEIGHT - (MARGIN + (HEIGHT - 2 * MARGIN) * pos.y);
    return pos;
}

void draw_points(Regression r) {
    for (size_t i = 0; i < r.N; ++i) {
        Vector2 pos = get_point_pos(r.xs[i], r.ys[i], r);
        DrawCircle(pos.x, pos.y, 5.0, RED);
    }
}

void draw_mse_lines(Regression r) {
    for (size_t i = 0; i < r.N; ++i) {
        double pred_y = r.w * r.xs[i] + r.b;
        Vector2 pred_pos = get_point_pos(r.xs[i], pred_y, r);
        Vector2 pos = get_point_pos(r.xs[i], r.ys[i], r);
        DrawLineEx((Vector2){pred_pos.x, pred_pos.y}, (Vector2){pos.x, pos.y},
                   3.0, BLACK);
    }
}

void draw_mse_squares(Regression r) {
    for (size_t i = 0; i < r.N; ++i) {
        double pred_y = r.w * r.xs[i] + r.b;
        Vector2 pred_pos = get_point_pos(r.xs[i], pred_y, r);
        Vector2 pos = get_point_pos(r.xs[i], r.ys[i], r);
        DrawRectangle(pos.x, pos.y, pred_pos.y - pos.y, pred_pos.y - pos.y,
                      pred_pos.y - pos.y < 0 ? (Color){255, 0, 0, 20}
                                             : (Color){0, 0, 255, 20});
    }
}

void draw_line(Regression r) {
    DrawLineEx(get_point_pos(r.min.x, r.w * r.min.x + r.b, r),
               get_point_pos(r.max.x, r.w * r.max.x + r.b, r), 3.0, BLUE);
}

int main(void) {
    srand(time(NULL));

    double xs[] = {1.0, 2.0, 3.0, 2.5, 1.7, 11.1, 7.3, 6.1, 5.5, 7.5};
    double ys[] = {1.0, 2.0, 3.0, 3.1, 0.9, 10, 9.2, 5.7, 4.3, 8.2};

    Regression r = regression_new(xs, ys, sizeof(xs) / sizeof(double));
    printf("max: %f, %f\n", r.max.x, r.max.y);
    printf("min: %f, %f\n", r.min.x, r.min.y);

    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(WIDTH, HEIGHT, "window");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(WHITE);

        regression_step(&r);
        draw_axis();
        draw_line(r);
        draw_points(r);
        // draw_mse_lines(r);
        draw_mse_squares(r);
        EndDrawing();
    }

    CloseWindow();
}
