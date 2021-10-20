#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif

#include "cubemap_texture.h"

#include <glad/glad.h>
#include <stb_image.h>

#include <vector>

#include "cg_exception.h"

const std::vector<std::pair<uint32_t, std::string>> CubemapTexture::kTypes = {
    {GL_TEXTURE_CUBE_MAP_NEGATIVE_X, "negx"},
    {GL_TEXTURE_CUBE_MAP_POSITIVE_X, "posx"},
    {GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, "negy"},
    {GL_TEXTURE_CUBE_MAP_POSITIVE_Y, "posy"},
    {GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, "negz"},
    {GL_TEXTURE_CUBE_MAP_POSITIVE_Z, "posz"},
};

void CubemapTexture::Load() {
  glGenTextures(1, &id_);
  glBindTexture(GL_TEXTURE_CUBE_MAP, id_);

  for (unsigned int i = 0; i < kTypes.size(); i++) {
    auto image_path = path_ + "/" + kTypes[i].second + "." + ext_;
    int w, h, comp;
    unsigned char* image = stbi_load(image_path.c_str(), &w, &h, &comp, 0);
    if (image == nullptr) throw LoadPictureError(image_path.c_str());

    if (comp == 3)
      glTexImage2D(kTypes[i].first, 0, GL_RGB, w, h, 0, GL_RGB,
                   GL_UNSIGNED_BYTE, image);
    else if (comp == 4)
      glTexImage2D(kTypes[i].first, 0, GL_RGBA, w, h, 0, GL_RGBA,
                   GL_UNSIGNED_BYTE, image);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    stbi_image_free(image);
  }
}

CubemapTexture::CubemapTexture(const std::string& path, const std::string& ext)
    : path_(path), ext_(ext) {
  Load();
}

void CubemapTexture::Bind(uint32_t unit) {
  glActiveTexture(unit);
  glBindTexture(GL_TEXTURE_CUBE_MAP, id_);
}