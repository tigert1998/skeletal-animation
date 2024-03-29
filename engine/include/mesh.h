//
//  mesh.h
//  skeletal-animation
//
//  Created by tigertang on 2018/8/3.
//  Copyright © 2018 tigertang. All rights reserved.
//

#pragma once

#include <assimp/scene.h>

#include <glm/glm.hpp>
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

  void Draw(Shader *shader_ptr, int num_instances) const;

  inline std::string name() { return name_; }

  void AppendTransform(glm::mat4 transform);

  inline glm::vec3 center(glm::mat4 *bone_matrix) {
    glm::mat4 transform =
        bone_matrix == nullptr || !has_bone_ ? transforms_[0] : *bone_matrix;
    return transform * glm::vec4(center_, 1);
  }

  inline uint32_t vao() { return vao_; }

  inline glm::vec3 max() { return max_; }
  inline glm::vec3 min() { return min_; }

 private:
  std::vector<glm::mat4> transforms_;
  uint32_t vao_, vbo_, ebo_, indices_size_;
  std::string name_;
  bool has_bone_ = false;
  glm::vec3 center_, min_, max_;

  struct TextureRecord {
    bool enabled;
    uint32_t id;
    int32_t op;
    float blend;
    glm::vec3 base_color;
  };

#define REGISTER(name)                        \
  {                                           \
#name, { false, 0, -1, -1, glm::vec3(0) } \
  }

  std::map<std::string, TextureRecord> textures_ = {
      REGISTER(DIFFUSE),
      REGISTER(AMBIENT),
      REGISTER(EMISSIVE),
      REGISTER(BASE_COLOR),
  };

#undef REGISTER
};
