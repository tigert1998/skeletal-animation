//
//  model.cpp
//  skeletal-animation
//
//  Created by tigertang on 2018/8/2.
//  Copyright © 2018 tigertang. All rights reserved.
//

#include "sprite_model.h"

#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <glad/glad.h>

#include <assimp/Importer.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "utils.h"

using std::make_shared;
using std::pair;
using std::shared_ptr;
using std::string;
using std::vector;
using namespace Assimp;
using namespace glm;

SpriteModel::SpriteModel(const std::string &path,
                         const std::vector<std::string> &filtered_node_names)
    : directory_path_(ParentPath(ParentPath(path))),
      filtered_node_names_(filtered_node_names) {
  scene_ = aiImportFile(path.c_str(), aiProcess_GlobalScale |
                                          aiProcess_CalcTangentSpace |
                                          aiProcess_Triangulate);
  shader_ptr_ = shared_ptr<Shader>(
      new Shader(Shader::SRC, SpriteModel::kVsSource, SpriteModel::kFsSource));

  animation_channel_map_.clear();
  for (int i = 0; i < scene_->mNumAnimations; i++) {
    auto animation = scene_->mAnimations[i];
    for (int j = 0; j < animation->mNumChannels; j++) {
      auto channel = animation->mChannels[j];
      animation_channel_map_[pair<uint32_t, string>(
          i, channel->mNodeName.C_Str())] = j;
    }
  }

  RecursivelyInitNodes(scene_->mRootNode);
  bone_matrices_.resize(bone_namer_.total());
}

SpriteModel::~SpriteModel() { aiReleaseImport(scene_); }

bool SpriteModel::NodeShouldBeFiltered(const std::string &name) {
  for (int i = 0; i < filtered_node_names_.size(); i++) {
    if (name == filtered_node_names_[i]) return true;
  }
  return false;
}

void SpriteModel::RecursivelyInitNodes(aiNode *node) {
  if (!NodeShouldBeFiltered(node->mName.C_Str())) {
    for (int i = 0; i < node->mNumMeshes; i++) {
      auto mesh = scene_->mMeshes[node->mMeshes[i]];
      mesh_ptrs_.emplace_back(make_shared<Mesh>(directory_path_, mesh, scene_,
                                                bone_namer_, bone_offsets_));
    }
  }
  for (int i = 0; i < node->mNumChildren; i++) {
    RecursivelyInitNodes(node->mChildren[i]);
  }
}

glm::mat4 SpriteModel::InterpolateTranslationMatrix(aiVectorKey *keys,
                                                    uint32_t n, double ticks) {
  static auto mat4_from_aivector3d = [](aiVector3D vector) -> mat4 {
    return translate(mat4(1), vec3(vector.x, vector.y, vector.z));
  };
  if (n == 0) return mat4(1);
  if (n == 1) return mat4_from_aivector3d(keys->mValue);
  if (ticks <= keys[0].mTime) return mat4_from_aivector3d(keys[0].mValue);
  if (keys[n - 1].mTime <= ticks)
    return mat4_from_aivector3d(keys[n - 1].mValue);

  aiVectorKey anchor;
  anchor.mTime = ticks;
  auto right_ptr = std::upper_bound(
      keys, keys + n, anchor, [](const aiVectorKey &a, const aiVectorKey &b) {
        return a.mTime < b.mTime;
      });
  auto left_ptr = right_ptr - 1;

  float factor =
      (ticks - left_ptr->mTime) / (right_ptr->mTime - left_ptr->mTime);
  return mat4_from_aivector3d(left_ptr->mValue * (1.0f - factor) +
                              right_ptr->mValue * factor);
}

glm::mat4 SpriteModel::InterpolateRotationMatrix(aiQuatKey *keys, uint32_t n,
                                                 double ticks) {
  static auto mat4_from_aiquaternion = [](aiQuaternion quaternion) -> mat4 {
    auto rotation_matrix = quaternion.GetMatrix();
    mat4 res(1);
    for (int i = 0; i < 3; i++)
      for (int j = 0; j < 3; j++) res[j][i] = rotation_matrix[i][j];
    return res;
  };
  if (n == 0) return mat4(1);
  if (n == 1) return mat4_from_aiquaternion(keys->mValue);
  if (ticks <= keys[0].mTime) return mat4_from_aiquaternion(keys[0].mValue);
  if (keys[n - 1].mTime <= ticks)
    return mat4_from_aiquaternion(keys[n - 1].mValue);

  aiQuatKey anchor;
  anchor.mTime = ticks;
  auto right_ptr = std::upper_bound(
      keys, keys + n, anchor,
      [](const aiQuatKey &a, const aiQuatKey &b) { return a.mTime < b.mTime; });
  auto left_ptr = right_ptr - 1;

  double factor =
      (ticks - left_ptr->mTime) / (right_ptr->mTime - left_ptr->mTime);
  aiQuaternion out;
  aiQuaternion::Interpolate(out, left_ptr->mValue, right_ptr->mValue, factor);
  return mat4_from_aiquaternion(out);
}

glm::mat4 SpriteModel::InterpolateScalingMatrix(aiVectorKey *keys, uint32_t n,
                                                double ticks) {
  static auto mat4_from_aivector3d = [](aiVector3D vector) -> mat4 {
    return scale(mat4(1), vec3(vector.x, vector.y, vector.z));
  };
  if (n == 0) return mat4(1);
  if (n == 1) return mat4_from_aivector3d(keys->mValue);
  if (ticks <= keys[0].mTime) return mat4_from_aivector3d(keys[0].mValue);
  if (keys[n - 1].mTime <= ticks)
    return mat4_from_aivector3d(keys[n - 1].mValue);

  aiVectorKey anchor;
  anchor.mTime = ticks;
  auto right_ptr = std::upper_bound(
      keys, keys + n, anchor, [](const aiVectorKey &a, const aiVectorKey &b) {
        return a.mTime < b.mTime;
      });
  auto left_ptr = right_ptr - 1;

  float factor =
      (ticks - left_ptr->mTime) / (right_ptr->mTime - left_ptr->mTime);
  return mat4_from_aivector3d(left_ptr->mValue * (1.0f - factor) +
                              right_ptr->mValue * factor);
}

int SpriteModel::NumAnimations() const { return scene_->mNumAnimations; }

void SpriteModel::RecursivelyUpdateBoneMatrices(int animation_id, aiNode *node,
                                                glm::mat4 transform,
                                                double ticks) {
  string node_name = node->mName.C_Str();
  auto animation = scene_->mAnimations[animation_id];
  mat4 current_transform;
  if (animation_channel_map_.count(
          pair<uint32_t, string>(animation_id, node_name))) {
    uint32_t channel_id =
        animation_channel_map_[pair<uint32_t, string>(animation_id, node_name)];
    auto channel = animation->mChannels[channel_id];

    // translation matrix
    mat4 translation_matrix = InterpolateTranslationMatrix(
        channel->mPositionKeys, channel->mNumPositionKeys, ticks);
    // rotation matrix
    mat4 rotation_matrix = InterpolateRotationMatrix(
        channel->mRotationKeys, channel->mNumRotationKeys, ticks);
    // scaling matrix
    mat4 scaling_matrix = InterpolateScalingMatrix(
        channel->mScalingKeys, channel->mNumScalingKeys, ticks);

    current_transform = translation_matrix * rotation_matrix * scaling_matrix;
  } else {
    current_transform = Mat4FromAimatrix4x4(node->mTransformation);
  }
  if (bone_namer_.map().count(node_name)) {
    uint32_t i = bone_namer_.map()[node_name];
    bone_matrices_[i] = transform * current_transform * bone_offsets_[i];
  }
  for (int i = 0; i < node->mNumChildren; i++) {
    RecursivelyUpdateBoneMatrices(animation_id, node->mChildren[i],
                                  transform * current_transform, ticks);
  }
}

void SpriteModel::Draw(uint32_t animation_id, double time, Camera *camera_ptr,
                       LightSources *light_sources, mat4 model_matrix) {
  RecursivelyUpdateBoneMatrices(
      animation_id, scene_->mRootNode, mat4(1),
      time * scene_->mAnimations[animation_id]->mTicksPerSecond);
  InternalDraw(true, camera_ptr, light_sources, model_matrix);
}

void SpriteModel::Draw(Camera *camera_ptr, LightSources *light_sources,
                       mat4 model_matrix) {
  InternalDraw(false, camera_ptr, light_sources, model_matrix);
}

void SpriteModel::InternalDraw(bool animated, Camera *camera_ptr,
                               LightSources *light_sources,
                               glm::mat4 model_matrix) {
  shader_ptr_->Use();
  if (light_sources != nullptr) {
    light_sources->Set(shader_ptr_.get());
  }
  shader_ptr_->SetUniform<mat4>("uModelMatrix", model_matrix);
  shader_ptr_->SetUniform<mat4>("uViewMatrix", camera_ptr->view_matrix());
  shader_ptr_->SetUniform<mat4>("uProjectionMatrix",
                                camera_ptr->projection_matrix());
  shader_ptr_->SetUniform<vector<mat4>>("uBoneMatrices", bone_matrices_);
  shader_ptr_->SetUniform<int32_t>("uDefaultShading", default_shading_);

  for (const auto &mesh_ptr : mesh_ptrs_) {
    shader_ptr_->SetUniform<int32_t>("uAnimated", animated);
    mesh_ptr->Draw(shader_ptr_.get());
  }
}

void SpriteModel::set_default_shading(bool default_shading) {
  default_shading_ = default_shading;
}

const std::string SpriteModel::kVsSource = R"(
#version 410 core

const int MAX_BONES = 100;

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in ivec4 aBoneIDs0;
layout (location = 4) in ivec4 aBoneIDs1;
layout (location = 5) in ivec4 aBoneIDs2;
layout (location = 6) in vec4 aBoneWeights0;
layout (location = 7) in vec4 aBoneWeights1;
layout (location = 8) in vec4 aBoneWeights2;

out vec3 vPosition;
out vec2 vTexCoord;
out vec3 vNormal;

uniform bool uAnimated;
uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;
uniform mat4 uBoneMatrices[MAX_BONES];

mat4 CalcBoneMatrix() {
    mat4 boneMatrix = mat4(0);
    for (int i = 0; i < 4; i++) {
        if (aBoneIDs0[i] < 0) return boneMatrix;
        boneMatrix += uBoneMatrices[aBoneIDs0[i]] * aBoneWeights0[i];
    }
    for (int i = 0; i < 4; i++) {
        if (aBoneIDs1[i] < 0) return boneMatrix;
        boneMatrix += uBoneMatrices[aBoneIDs1[i]] * aBoneWeights1[i];
    }
    for (int i = 0; i < 4; i++) {
        if (aBoneIDs2[i] < 0) return boneMatrix;
        boneMatrix += uBoneMatrices[aBoneIDs2[i]] * aBoneWeights2[i];
    }
    return boneMatrix;
}

void main() {
    mat4 boneMatrix = mat4(1);
    if (uAnimated) {
      boneMatrix = CalcBoneMatrix();
    }
    gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * boneMatrix * vec4(aPosition, 1);
    vPosition = vec3(uModelMatrix * boneMatrix * vec4(aPosition, 1));
    vTexCoord = aTexCoord;
    vNormal = vec3(uModelMatrix * boneMatrix * vec4(aNormal, 0));
}
)";

const std::string SpriteModel::kFsSource = R"(
#version 410 core

const float zero = 0.00000001;

in vec3 vPosition;
in vec2 vTexCoord;
in vec3 vNormal;

#define REG_TEX(name) \
    uniform bool u##name##Enabled;      \
    uniform sampler2D u##name##Texture;

REG_TEX(Ambient)
REG_TEX(Diffuse)
REG_TEX(Emissive)
REG_TEX(BaseColor)

#undef REG_TEX

struct DirectionalLight {
    vec3 dir;
    vec3 color;
};

struct PointLight {
    vec3 pos;
    vec3 color;
};

#define REG_LIGHT(name, count) \
    uniform int u##name##LightCount;      \
    uniform name##Light u##name##Lights[count];

REG_LIGHT(Directional, 1)
REG_LIGHT(Point, 8)

#undef REG_LIGHT

uniform bool uDefaultShading;

out vec4 fragColor;

vec3 calcDiffuse(vec3 raw) {
    vec3 normal = normalize(vNormal);
    vec3 ans = vec3(0);
    for (int i = 0; i < uDirectionalLightCount; i++) {
        vec3 dir = normalize(-uDirectionalLights[i].dir);
        ans += max(dot(normal, dir), 0.0) * uDirectionalLights[i].color;
    }
    for (int i = 0; i < uPointLightCount; i++) {
        vec3 dir = normalize(uPointLights[i].pos - vPosition);
        ans += max(dot(normal, dir), 0.0) * uPointLights[i].color;
    }
    return ans * raw;
}

vec3 defaultShading() {
    vec3 color = vec3(0.9608f, 0.6784f, 0.2588f);
    return color * 0.2 + calcDiffuse(color);
}

void main() {
    if (uDefaultShading) {
        fragColor = vec4(defaultShading(), 1);
        return;
    }
    vec3 color = vec3(0);
    float alpha = 1.0f;
    if (uAmbientEnabled) {
        color += texture(uAmbientTexture, vTexCoord).rgb;
    }
    if (uDiffuseEnabled) {
        vec4 diffuseTexture = texture(uDiffuseTexture, vTexCoord); 
        color += calcDiffuse(diffuseTexture.rgb);
        alpha = diffuseTexture.a;
    }
    if (uEmissiveEnabled) {
        color += texture(uEmissiveTexture, vTexCoord).rgb;
    }
    if (uBaseColorEnabled) {
        color += texture(uBaseColorTexture, vTexCoord).rgb;
    }
    fragColor = vec4(color, alpha);
}
)";