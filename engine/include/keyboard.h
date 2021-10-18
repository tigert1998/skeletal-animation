//
//  keyboard.h
//  skinned-animation
//
//  Created by tigertang on 2018/8/4.
//  Copyright © 2018 tigertang. All rights reserved.
//

#pragma once

#include <vector>
#include <functional>
#include <bitset>

class Keyboard {
private:
    static constexpr int kTotal = 1024;
    std::bitset<kTotal> key_pressed_;
    std::vector<std::function<void(std::bitset<kTotal>, double)>> callbacks_;
    
public:
    using KeyboardState = std::bitset<kTotal>;
    static Keyboard shared;
    Keyboard();
    void Trigger(int key, int action);
    void Elapse(double time) const;
    void Register(std::function<void(KeyboardState, double)> callback);
    
};
