//
//  model.h
//  skeletal-animation
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

class Model {
 public:
  Model() = delete;
  Model(const std::string &path,
        const std::vector<std::string> &filtered_node_names);
  Model(const std::string &path);
  ~Model();
  void Draw(Camera *camera_ptr, LightSources *light_sources,
            glm::mat4 model_matrix);
  void Draw(Camera *camera_ptr, LightSources *light_sources,
            const std::vector<glm::mat4> &model_matrices);
  void Draw(uint32_t animation_id, double time, Camera *camera_ptr,
            LightSources *light_sources, glm::mat4 model_matrix);
  int NumAnimations() const;
  void set_default_shading(bool default_shading);
  inline bool default_shading() { return default_shading_; }

  inline glm::vec3 min() { return min_; }
  inline glm::vec3 max() { return max_; }

 private:
  std::vector<std::string> filtered_node_names_;
  std::string directory_path_;
  std::vector<std::shared_ptr<Mesh>> mesh_ptrs_;
  const aiScene *scene_;
  std::shared_ptr<Shader> shader_ptr_;
  Namer bone_namer_;
  uint32_t vbo_;
  glm::vec3 min_, max_;
  std::vector<glm::mat4> bone_matrices_, bone_offsets_;
  std::map<std::pair<uint32_t, std::string>, uint32_t> animation_channel_map_;
  bool default_shading_ = false;

  void RecursivelyInitNodes(aiNode *node, glm::mat4 parent_transform);
  void RecursivelyUpdateBoneMatrices(int animation_id, aiNode *node,
                                     glm::mat4 transform, double ticks);

  static glm::mat4 InterpolateTranslationMatrix(aiVectorKey *keys, uint32_t n,
                                                double ticks);
  static glm::mat4 InterpolateRotationMatrix(aiQuatKey *keys, uint32_t n,
                                             double ticks);
  static glm::mat4 InterpolateScalingMatrix(aiVectorKey *keys, uint32_t n,
                                            double ticks);

  bool NodeShouldBeFiltered(const std::string &name);
  void InternalDraw(bool animated, Camera *camera_ptr,
                    LightSources *light_sources,
                    const std::vector<glm::mat4> &model_matrices,
                    bool sort_meshes);

  static const std::string kVsSource;
  static const std::string kFsSource;
  static std::shared_ptr<Shader> kShader;
};
