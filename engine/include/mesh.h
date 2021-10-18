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

  std::map<std::string, std::pair<bool, uint32_t>> textures_ = {
      {"DIFFUSE", {false, 0}},
      {"AMBIENT", {false, 0}},
  };
};
