#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "regression.h"

#define NUM_POINTS 40
#define GRAPH_MARGIN 5
#define WIDTH 800
#define HEIGHT (850 + GRAPH_MARGIN)
#define GRAPH_WIDTH (WIDTH - 2 * GRAPH_MARGIN)
#define GRAPH_HEIGHT (WIDTH - 2 * GRAPH_MARGIN)
#define AXIS_GRAPH_WIDTH 2.
#define AXIS_COLOR \
  CLITERAL(Color) { 100, 100, 100, 255 }
#define LINE_COLOR BLUE
#define BORDER_WIDTH 2
void draw_axis(void) {
  // X axis
  DrawLineEx((Vector2){GRAPH_MARGIN, GRAPH_HEIGHT + GRAPH_MARGIN},
             (Vector2){GRAPH_WIDTH - GRAPH_MARGIN, GRAPH_HEIGHT + GRAPH_MARGIN},
             AXIS_GRAPH_WIDTH, AXIS_COLOR);

#ifdef AXIS_TRIANGLES
  Vector2 v1 =
      (Vector2){GRAPH_WIDTH - GRAPH_MARGIN + 2, GRAPH_HEIGHT - GRAPH_MARGIN};
  Vector2 v2 = (Vector2){v1.x - 8.66, v1.y + 5};
  Vector2 v3 = (Vector2){v1.x - 8.66, v1.y - 5};
  DrawText("x", v1.x - 10, v1.y - 20, 12, AXIS_COLOR);
  DrawTriangle(v3, v2, v1, AXIS_COLOR);
#endif

  // Y axis
  DrawLineEx((Vector2){GRAPH_MARGIN, GRAPH_MARGIN},
             (Vector2){GRAPH_MARGIN, GRAPH_HEIGHT + GRAPH_MARGIN},
             AXIS_GRAPH_WIDTH, AXIS_COLOR);
#ifdef AXIS_TRIANGLES
  v1 = (Vector2){GRAPH_MARGIN, GRAPH_MARGIN - 2};
  v2 = (Vector2){v1.x - 5, v1.y + 8.66};
  v3 = (Vector2){v1.x + 5, v1.y + 8.66};
  DrawText("y", v1.x + 10, v1.y, 12, AXIS_COLOR);
  DrawTriangle(v1, v2, v3, AXIS_COLOR);
#endif
}

Vector2 get_point_pos(double x, double y, Regression r) {
  Vector2 pos = {x, y};

  // X
  pos.x = (pos.x - r.min.x) / (r.max.x - r.min.x);
  pos.x = GRAPH_MARGIN + (GRAPH_WIDTH - 2 * GRAPH_MARGIN) * pos.x;

  // Y
  pos.y = (pos.y - r.min.y) / (r.max.y - r.min.y);
  pos.y = GRAPH_HEIGHT + GRAPH_MARGIN - (GRAPH_HEIGHT * pos.y);

  return pos;
}

void draw_points(Regression r) {
  for (size_t i = 0; i < r.N; ++i) {
    Vector2 pos = get_point_pos(r.xs[i], r.ys[i], r);
    DrawCircle(pos.x, pos.y, 3.0, RED);
  }
}

void draw_mse_lines(Regression r) {
  for (size_t i = 0; i < r.N; ++i) {
    double pred_y = r.w * r.xs[i] + r.b;
    Vector2 pred_pos = get_point_pos(r.xs[i], pred_y, r);
    Vector2 pos = get_point_pos(r.xs[i], r.ys[i], r);
    DrawLineEx((Vector2){pred_pos.x - 1, pred_pos.y},
               (Vector2){pos.x - 1, pos.y}, 2.0, BLACK);
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
             get_point_pos(r.max.x, r.w * r.max.x + r.b, r), AXIS_GRAPH_WIDTH,
             LINE_COLOR);
}

void draw_stats(Regression r) {
  Rectangle rect =
      (Rectangle){GRAPH_WIDTH - 75, GRAPH_HEIGHT - 75, 75.0f, 75.0f};

  BeginScissorMode(rect.x, rect.y, rect.width, rect.height);
  DrawText(TextFormat("w: %.2f", r.w), 5 + rect.x, 5 + rect.y + 20 * 0, 14,
           BLACK);
  DrawText(TextFormat("b: %.2f", r.b), 5 + rect.x, 5 + rect.y + 20 * 1, 14,
           BLACK);
  DrawText(TextFormat("l: %.2f", mse_loss(r)), 5 + rect.x, 5 + rect.y + 20 * 2,
           14, BLACK);
  EndScissorMode();

  GuiGroupBox(rect, "Stats");
}

void draw_control_panel(bool *cont) {}

void generate_data(double xs[NUM_POINTS], double ys[NUM_POINTS]) {
  double m = drand(0.5, 3);
  for (size_t i = 0; i < NUM_POINTS; ++i) {
    xs[i] = i;
    ys[i] = m * xs[i] + drand(-3, 3);
  }
}

int main(void) {
  SetConfigFlags(FLAG_MSAA_4X_HINT);
  InitWindow(WIDTH, HEIGHT, "Regression");
  SetTargetFPS(60);

  srand(time(NULL));
  double xs[NUM_POINTS] = {0};
  double ys[NUM_POINTS] = {0};

  generate_data(xs, ys);

  Regression r = regression_new(xs, ys, NUM_POINTS);

  bool cont = false;

  while (!WindowShouldClose()) {
    BeginDrawing();
    {
      ClearBackground(WHITE);

      Rectangle border_rect = (Rectangle){
          GRAPH_MARGIN - BORDER_WIDTH, GRAPH_MARGIN - BORDER_WIDTH,
          GRAPH_WIDTH + 2 * BORDER_WIDTH, GRAPH_HEIGHT + 2 * BORDER_WIDTH};

      Rectangle main_panel =
          (Rectangle){GRAPH_MARGIN, GRAPH_MARGIN, GRAPH_WIDTH, GRAPH_HEIGHT};

      BeginScissorMode(main_panel.x, main_panel.y, main_panel.width,
                       main_panel.height);
      draw_axis();
      draw_line(r);
      draw_mse_squares(r);
      // draw_mse_lines(r);
      draw_points(r);
      draw_stats(r);

      if (GuiButton((Rectangle){GRAPH_MARGIN + 0 * (100 + GRAPH_MARGIN),
                                GRAPH_HEIGHT + 2 * GRAPH_MARGIN, 100, 50},
                    "Reset")) {
        generate_data(xs, ys);
        r = regression_new(xs, ys, sizeof(xs) / sizeof(double));
        cont = false;
      }

      if (GuiButton((Rectangle){GRAPH_MARGIN + 1 * (100 + GRAPH_MARGIN),
                                GRAPH_HEIGHT + 2 * GRAPH_MARGIN, 100, 50},
                    "Step") ||
          cont || GetKeyPressed() == KEY_SPACE) {
        regression_step(&r);
      }

      if (GuiButton((Rectangle){GRAPH_MARGIN + 2 * (100 + GRAPH_MARGIN),
                                GRAPH_HEIGHT + 2 * GRAPH_MARGIN, 100, 50},
                    "Continue")) {
        cont = true;
      }

      EndScissorMode();

      GuiGroupBox(main_panel, "Regression");
    }
    EndDrawing();
  }

  CloseWindow();
}
