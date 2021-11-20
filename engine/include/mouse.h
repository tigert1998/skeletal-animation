//
//  keyboard.h
//  skeletal-animation
//
//  Created by tigertang on 2018/8/4.
//  Copyright © 2018 tigertang. All rights reserved.
//

#pragma once

#include <bitset>
#include <functional>
#include <vector>

class Mouse {
 private:
  static constexpr int kTotal = 16;
  std::bitset<kTotal> key_pressed_;
  std::vector<std::function<void(std::bitset<kTotal>, double, double, double)>>
      callbacks_;
  double x_ = 0, y_ = 0;

 public:
  using MouseState = std::bitset<kTotal>;
  static Mouse shared;
  Mouse();
  void Trigger(int key, int action);
  void Elapse(double delta_time) const;
  void Move(double, double);
  void Register(
      std::function<void(MouseState, double, double, double)> callback);
};
