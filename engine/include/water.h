#pragma once

#include <vector>

#include "camera.h"

class Water {
 private:
  std::vector<float> u_, v_, buf_;
  int height_, width_;
  float length_;
  uint32_t vao_, vbo_, ebo_;

  int Index(int x, int y);
  float GetU(int x, int y);

 public:
  Water(int height, int width, float length);
  ~Water();
  void StepSimulation(double delta_time);
  void Draw(Camera *camera);
};