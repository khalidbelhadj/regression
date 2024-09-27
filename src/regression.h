#pragma once

#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "raylib.h"

extern double STEP_SIZE;

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

double drand(double min, double max);
double mse_loss(Regression r);
double d_mse_loss_w(Regression r);
double d_mse_loss_b(Regression r);
Regression regression_new(double *xs, double *ys, int N);
void regression_step(Regression *r);
