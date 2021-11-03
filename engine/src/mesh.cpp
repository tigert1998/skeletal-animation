//
//  mesh.cpp
//  skeletal-animation
//
//  Created by tigertang on 2018/8/3.
//  Copyright © 2018 tigertang. All rights reserved.
//

#include "mesh.h"

#include <glad/glad.h>
#include <glog/logging.h>

#include <algorithm>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#include "texture_manager.h"
#include "utils.h"

using namespace glm;

constexpr int kMaxBonesPerVertex = 12;

using VertexWithBones = Vertex<kMaxBonesPerVertex>;

Mesh::Mesh(const std::string &directory_path, aiMesh *mesh,
           const aiScene *scene, Namer &bone_namer,
           std::vector<glm::mat4> &bone_offsets) {
  std::vector<VertexWithBones> vertices;
  std::vector<uint32_t> indices;
  name_ = mesh->mName.C_Str();

  auto path = directory_path;
  {
    aiString material_texture_path;
    auto material = scene->mMaterials[mesh->mMaterialIndex];
#define INTERNAL_ADD_TEXTURE(name)                                         \
  do {                                                                     \
    textures_[#name].enabled = true;                                       \
    material->GetTexture(aiTextureType_##name, 0, &material_texture_path); \
    auto basename = BaseName(material_texture_path.C_Str());               \
    auto item = path + "/textures/" + basename;                            \
    textures_[#name].id = TextureManager::LoadTexture(item);               \
  } while (0)
#define TRY_ADD_TEXTURE(name)                                 \
  if (material->GetTextureCount(aiTextureType_##name) >= 1) { \
    INTERNAL_ADD_TEXTURE(name);                               \
  }
#define TRY_ADD_TEXTURE_WITH_BASE_COLOR(name)                              \
  if (material->GetTextureCount(aiTextureType_##name) >= 1) {              \
    INTERNAL_ADD_TEXTURE(name);                                            \
    material->Get(AI_MATKEY_TEXBLEND_##name(0), textures_[#name].blend);   \
    material->Get(AI_MATKEY_TEXOP_##name(0), textures_[#name].op);         \
    aiColor3D color(0.f, 0.f, 0.f);                                        \
    material->Get(AI_MATKEY_COLOR_##name, color);                          \
    textures_[#name].base_color = glm::vec3(color[0], color[1], color[2]); \
  }
    TRY_ADD_TEXTURE_WITH_BASE_COLOR(DIFFUSE);
    TRY_ADD_TEXTURE_WITH_BASE_COLOR(AMBIENT);
    TRY_ADD_TEXTURE_WITH_BASE_COLOR(EMISSIVE);
    TRY_ADD_TEXTURE(BASE_COLOR);
#undef INTERNAL_ADD_TEXTURE
#undef TRY_ADD_TEXTURE
#undef TRY_ADD_TEXTURE_WITH_BASE_COLOR
  }

  LOG(INFO) << "\"" << name() << "\": #vertices: " << mesh->mNumVertices
            << ", #faces: " << mesh->mNumFaces;

  vertices.reserve(mesh->mNumVertices);
  indices.reserve(mesh->mNumFaces * 3);
  for (int i = 0; i < mesh->mNumVertices; i++) {
    auto vertex = VertexWithBones();
    vertex.position =
        vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
    if (mesh->HasTextureCoords(0)) {
      vertex.tex_coord =
          vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
    }
    vertex.normal =
        vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
    vertices.push_back(vertex);
  }
  for (int i = 0; i < mesh->mNumFaces; i++) {
    auto face = mesh->mFaces[i];
    for (int j = 0; j < face.mNumIndices; j++)
      indices.push_back(face.mIndices[j]);
  }
  indices_size_ = (uint32_t)indices.size();

  for (int i = 0; i < mesh->mNumBones; i++) {
    auto bone = mesh->mBones[i];
    auto id = bone_namer.Name(bone->mName.C_Str());

    bone_offsets.resize(max(id + 1, (uint32_t)bone_offsets.size()));
    bone_offsets[id] = Mat4FromAimatrix4x4(bone->mOffsetMatrix);

    for (int j = 0; j < bone->mNumWeights; j++) {
      auto weight = bone->mWeights[j];
      vertices[weight.mVertexId].AddBone(id, weight.mWeight);
      has_bone_ = true;
    }
  }

  glGenVertexArrays(1, &vao_);
  glGenBuffers(1, &vbo_);
  glGenBuffers(1, &ebo_);

  glBindVertexArray(vao_);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(VertexWithBones) * vertices.size(),
               vertices.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * indices.size(),
               indices.data(), GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(VertexWithBones),
                        (void *)offsetof(VertexWithBones, position));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(VertexWithBones),
                        (void *)offsetof(VertexWithBones, tex_coord));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 3, GL_FLOAT, false, sizeof(VertexWithBones),
                        (void *)offsetof(VertexWithBones, normal));

  for (int i = 0; i < kMaxBonesPerVertex / 4; i++) {
    int location = 3 + i;
    glEnableVertexAttribArray(location);
    glVertexAttribIPointer(
        location, 4, GL_INT, sizeof(VertexWithBones),
        (void *)(offsetof(VertexWithBones, bone_ids) + i * 4 * sizeof(int)));
  }
  for (int i = 0; i < kMaxBonesPerVertex / 4; i++) {
    int location = 3 + kMaxBonesPerVertex / 4 + i;
    glEnableVertexAttribArray(location);
    glVertexAttribPointer(location, 4, GL_FLOAT, false, sizeof(VertexWithBones),
                          (void *)(offsetof(VertexWithBones, bone_weights) +
                                   i * 4 * sizeof(float)));
  }

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::AppendTransform(glm::mat4 transform) {
  transforms_.push_back(transform);
}

Mesh::~Mesh() {
  glDeleteVertexArrays(1, &vao_);
  glDeleteBuffers(1, &vbo_);
  glDeleteBuffers(1, &ebo_);
  for (auto kv : textures_) {
    if (kv.second.enabled) {
      glDeleteTextures(1, &kv.second.id);
    }
  }
}

void Mesh::Draw(Shader *shader_ptr) const {
  if (!has_bone_) {
    shader_ptr->SetUniform<int32_t>("uAnimated", 0);
  }

  int tot = 0;
  for (auto kv : textures_) {
    bool enabled = kv.second.enabled;
    std::string name = SnakeToPascal(kv.first);
    shader_ptr->SetUniform<int32_t>(std::string("u") + name + "Enabled",
                                    enabled);
    if (enabled) {
      glActiveTexture(GL_TEXTURE0 + tot);
      glBindTexture(GL_TEXTURE_2D, kv.second.id);
      shader_ptr->SetUniform<int32_t>(std::string("u") + name + "Texture", tot);
      tot++;
    }
  }

  CHECK(transforms_.size() == 1);
  shader_ptr->SetUniform<glm::mat4>("uTransform", transforms_[0]);

  glBindVertexArray(vao_);
  glDrawElements(GL_TRIANGLES, indices_size_, GL_UNSIGNED_INT, nullptr);
  glBindVertexArray(0);
}
