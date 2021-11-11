#pragma once

#include <vector>

class Water {
 private:
  std::vector<float> u_, v_, buf_;
  int height_, width_;

  int Index(int x, int y);
  float GetU(int x, int y);

 public:
  Water(int height, int width);
  void StepSimulation(double delta_time);
};