//
//  namer.cpp
//  skinned-animation
//
//  Created by tigertang on 2018/8/6.
//

#include "namer.h"

Namer::Namer() {
    Clear();
}

void Namer::Clear() {
    map_.clear();
    total_ = 0;
}

uint32_t Namer::Name(const std::string &name) {
    if (map_.count(name))
        return map_[name];
    return map_[name] = total_++;
}

uint32_t Namer::total() const {
    return total_;
}

std::map<std::string, uint32_t> &Namer::map() {
    return map_;
}
