//
//  texture_manager.h
//  skeletal-animation
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

  static uint32_t LoadTexture(const std::string &path, uint32_t wrap);

  static uint32_t AllocateTexture(uint32_t height, uint32_t width,
                                  uint32_t format);
};
