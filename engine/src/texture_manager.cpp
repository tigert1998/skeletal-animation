//
//  texture_manager.cpp
//  skeletal-animation
//
//  Created by tigertang on 2018/8/3.
//  Copyright © 2018 tigertang. All rights reserved.
//

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif

#include "texture_manager.h"

#include <glad/glad.h>

#include <map>
#include <string>
#include <vector>

#include "cg_exception.h"
#include "stb_image.h"

using std::map;
using std::string;
using std::vector;

uint32_t TextureManager::LoadTexture(const std::string& path) {
  uint32_t texture;
  static map<string, uint32_t> memory;
  if (memory.count(path.c_str())) return memory[path.c_str()];
  int w, h, comp;
  stbi_set_flip_vertically_on_load(true);
  unsigned char* image = stbi_load(path.c_str(), &w, &h, &comp, 0);
  if (image == nullptr) throw LoadPictureError(path.c_str());

  glGenTextures(1, &texture);

  glBindTexture(GL_TEXTURE_2D, texture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  vector<float> border_color(4);
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color.data());

  if (comp == 3)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE,
                 image);
  else if (comp == 4)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 image);

  glGenerateMipmap(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);

  stbi_image_free(image);
  return memory[path.c_str()] = texture;
}
