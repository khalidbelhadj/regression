#include "regression.h"

double STEP_SIZE = 0.01;

double drand(double min, double max) {
  double range = max - min;
  return min + (rand() * range) / RAND_MAX;
}

double mse_loss(Regression r) {
  double loss = 0.0f;
  for (size_t i = 0; i < r.N; ++i) {
    double y_pred = r.w * r.xs[i] + r.b;
    loss += pow(r.ys[i] - y_pred, 2);
  }

  return loss / r.N;
}

double d_mse_loss_w(Regression r) {
  double dloss = 0;
  for (size_t i = 0; i < r.N; ++i) {
    double y_pred = r.w * r.xs[i] + r.b;
    dloss += r.xs[i] * (r.ys[i] - y_pred);
  }

  return -2.0f * dloss / r.N;
}

double d_mse_loss_b(Regression r) {
  double dloss = 0;
  for (size_t i = 0; i < r.N; ++i) {
    double y_pred = r.w * r.xs[i] + r.b;
    dloss += r.ys[i] - y_pred;
  }

  return 2.0f * dloss / r.N;
}

Regression regression_new(double *xs, double *ys, int N) {
  double min = 0.0;
  double max = 2.0;

  Regression r = {
      .N = N,
      .xs = xs,
      .ys = ys,
      .w = drand(min, max),
      .b = 0.0,
      .min = (Vector2){INFINITY, INFINITY},
      .max = (Vector2){-INFINITY, -INFINITY},
  };

  for (size_t i = 0; i < r.N; ++i) {
    double x = r.xs[i];
    double y = r.ys[i];

    if (x < r.min.x) r.min.x = x;
    if (y < r.min.y) r.min.y = y;

    if (x > r.max.x) r.max.x = x;
    if (y > r.max.y) r.max.y = y;
  }
  return r;
}

void regression_step(Regression *r) {
  double loss = mse_loss(*r);

  r->w -= STEP_SIZE * d_mse_loss_w(*r);
  r->b -= STEP_SIZE * d_mse_loss_b(*r);

  if (mse_loss(*r) - loss > 0) {
    STEP_SIZE *= 0.5;
  } else {
    STEP_SIZE *= 1.5;
  }
}
