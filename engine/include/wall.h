//
//  wall.h
//  skeletal-animation
//
//  Created by tigertang on 2018/8/5.
//  Copyright © 2018 tigertang. All rights reserved.
//

#pragma once

#include <stdint.h>

#include <memory>

#include "camera.h"
#include "shader.h"
#include "vertex.h"

class Wall {
 private:
  using VertexType = Vertex<0>;

  uint32_t vao_, vbo_;
  std::shared_ptr<Shader> shader_ptr_;

 public:
  Wall();
  ~Wall();
  void Draw(Camera *camera, glm::mat4 model_matrix) const;
};
