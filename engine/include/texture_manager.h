//
//  texture_manager.h
//  skinned-animation
//
//  Created by tigertang on 2018/8/3.
//  Copyright © 2018 tigertang. All rights reserved.
//

#pragma once

#include <stdint.h>

#include <string>

class TextureManager {
 public:
  static uint32_t LoadTexture(const std::string &path);
};
