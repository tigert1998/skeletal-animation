//
//  keyboard.cpp
//  skinned-animation
//
//  Created by tigertang on 2018/8/4.
//  Copyright © 2018 tigertang. All rights reserved.
//

#include <GLFW/glfw3.h>

#include "keyboard.h"

Keyboard Keyboard::shared = Keyboard();

Keyboard::Keyboard() = default;

void Keyboard::Trigger(int key, int action) {
    if(key < 0 || key >= Keyboard::kTotal)
        key = Keyboard::kTotal - 1;
    key_pressed_[key] = (action != GLFW_RELEASE);
}

void Keyboard::Elapse(double time) const {
    for (auto f : callbacks_)
        f(key_pressed_, time);
}

void Keyboard::Register(std::function<void(KeyboardState, double)> yield) {
    callbacks_.push_back(yield);
}
