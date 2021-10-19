//
//  mesh.h
//  skinned-animation
//
//  Created by tigertang on 2018/8/3.
//  Copyright © 2018 tigertang. All rights reserved.
//

#pragma once

#include <assimp/scene.h>

#include <memory>
#include <tuple>
#include <vector>

#include "namer.h"
#include "shader.h"
#include "vertex.h"

class Mesh {
 public:
  Mesh(const std::string &directory_path, aiMesh *mesh, const aiScene *scene,
       Namer &bone_namer, std::vector<glm::mat4> &bone_offsets);
  ~Mesh();
  void Draw(std::weak_ptr<Shader> shader_ptr) const;

 private:
  uint32_t vao_, vbo_, ebo_, indices_size_;

  struct TextureRecord {
    bool enabled;
    uint32_t id;
    float blend;
    glm::vec3 base_color;
  };

#define REGISTER(name)                   \
  {                                      \
#name, { false, 0, 0, glm::vec3(0) } \
  }

  std::map<std::string, TextureRecord> textures_ = {
      REGISTER(DIFFUSE),
      REGISTER(AMBIENT),
  };

#undef REGISTER
};
