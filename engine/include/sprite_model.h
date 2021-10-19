//
//  model.h
//  skinned-animation
//
//  Created by tigertang on 2018/8/2.
//  Copyright © 2018 tigertang. All rights reserved.
//

#pragma once

#include <assimp/scene.h>

#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <vector>

#include "camera.h"
#include "light_sources.h"
#include "mesh.h"
#include "namer.h"
#include "shader.h"

class SpriteModel {
 public:
  SpriteModel() = delete;
  SpriteModel(const std::string &path,
              const std::vector<std::string> &filtered_node_names);
  ~SpriteModel();
  void Draw(Camera *camera_ptr, LightSources *light_sources, glm::mat4 model_matrix);
  void Draw(uint32_t animation_id, double time, Camera *camera_ptr,
            LightSources *light_sources, glm::mat4 model_matrix);
  int NumAnimations() const;

 private:
  std::vector<std::string> filtered_node_names_;
  std::string directory_path_;
  std::vector<std::shared_ptr<Mesh>> mesh_ptrs_;
  const aiScene *scene_;
  std::shared_ptr<Shader> shader_ptr_;
  Namer bone_namer_;
  std::vector<glm::mat4> bone_matrices_, bone_offsets_;
  std::map<std::pair<uint32_t, std::string>, uint32_t> animation_channel_map_;

  void RecursivelyInitNodes(aiNode *node);
  void RecursivelyUpdateBoneMatrices(int animation_id, aiNode *node,
                                     glm::mat4 transform, double ticks);

  static glm::mat4 InterpolateTranslationMatrix(aiVectorKey *keys, uint32_t n,
                                                double ticks);
  static glm::mat4 InterpolateRotationMatrix(aiQuatKey *keys, uint32_t n,
                                             double ticks);
  static glm::mat4 InterpolateScalingMatrix(aiVectorKey *keys, uint32_t n,
                                            double ticks);

  bool NodeShouldBeFiltered(const std::string &name);

  static const std::string kVsSource;
  static const std::string kFsSource;
};
