#include "water.h"

Water::Water(int height, int width)
    : height_(height),
      width_(width),
      u_(height * width),
      v_(height * width),
      buf_(height * width) {}

int Water::Index(int x, int y) { return x * width_ + y; }

float Water::GetU(int x, int y) {
  if (x < 0) x = 0;
  if (x >= height_) x = height_ - 1;
  if (y < 0) y = 0;
  if (y >= width_) y = width_ - 1;
  auto idx = Index(x, y);
  return u_[idx];
}

void Water::StepSimulation(double delta_time) {
  const float c = 16;

  for (int i = 0; i < height_; i++) {
    for (int j = 0; j < width_; j++) {
      int idx = Index(i, j);
      float f =
          c * c *
          ((GetU(i - 1, j) + GetU(i + 1, j) + GetU(i, j - 1) + GetU(i, j + 1)) -
           4 * GetU(i, j));
      v_[idx] += f * delta_time;
      v_[idx] *= 0.995;
      buf_[idx] = GetU(i, j) + v_[idx] * delta_time;
    }
  }

  u_ = buf_;
}