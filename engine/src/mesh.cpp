//
//  mesh.cpp
//  skinned-animation
//
//  Created by tigertang on 2018/8/3.
//  Copyright © 2018 tigertang. All rights reserved.
//

#include "mesh.h"

#include <glad/glad.h>
#include <glog/logging.h>

#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>

#include "texture_manager.h"
#include "utils.h"

using namespace glm;

Mesh::Mesh(const std::string &directory_path, aiMesh *mesh,
           const aiScene *scene, Namer &bone_namer,
           std::vector<glm::mat4> &bone_offsets) {
  std::vector<Vertex> vertices;
  std::vector<uint32_t> indices;

  auto path = directory_path;
  {
    aiString material_texture_path;
    auto material = scene->mMaterials[mesh->mMaterialIndex];

#define TRY_ADD_TEXTURE_WITH_BASE_COLOR(name)                                  \
  if (material->GetTextureCount(aiTextureType_##name) >= 1) {                  \
    textures_[#name].enabled = true;                                           \
    material->GetTexture(aiTextureType_##name, 0, &material_texture_path);     \
    auto item = path + "/textures/" + BaseName(material_texture_path.C_Str()); \
    textures_[#name].id = TextureManager::LoadTexture(item);                   \
    material->Get(AI_MATKEY_TEXBLEND_##name(0), textures_[#name].blend);       \
    material->Get(AI_MATKEY_TEXOP_##name(0), textures_[#name].op);             \
    aiColor3D color(0.f, 0.f, 0.f);                                            \
    material->Get(AI_MATKEY_COLOR_##name, color);                              \
    textures_[#name].base_color = glm::vec3(color[0], color[1], color[2]);     \
  }
    TRY_ADD_TEXTURE_WITH_BASE_COLOR(DIFFUSE);
    TRY_ADD_TEXTURE_WITH_BASE_COLOR(AMBIENT);

#undef TRY_ADD_TEXTURE_WITH_BASE_COLOR
  }
  for (int i = 0; i < mesh->mNumVertices; i++) {
    auto vertex = Vertex();
    vertex.position =
        vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
    vertex.tex_coord =
        vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
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

    bone_offsets.resize(std::max(id + 1, (uint32_t)bone_offsets.size()));
    bone_offsets[id] = Mat4FromAimatrix4x4(bone->mOffsetMatrix);

    for (int j = 0; j < bone->mNumWeights; j++) {
      auto weight = bone->mWeights[j];
      vertices[weight.mVertexId].AddBone(id, weight.mWeight);
    }
  }

  glGenVertexArrays(1, &vao_);
  glGenBuffers(1, &vbo_);
  glGenBuffers(1, &ebo_);

  glBindVertexArray(vao_);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(),
               vertices.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * indices.size(),
               indices.data(), GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex),
                        (void *)offsetof(Vertex, position));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(Vertex),
                        (void *)offsetof(Vertex, tex_coord));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 3, GL_FLOAT, false, sizeof(Vertex),
                        (void *)offsetof(Vertex, normal));

  glEnableVertexAttribArray(3);
  glVertexAttribIPointer(
      3, 4, GL_INT, sizeof(Vertex),
      (void *)(offsetof(Vertex, bone_ids) + 0 * sizeof(int)));
  glEnableVertexAttribArray(4);
  glVertexAttribIPointer(
      4, 4, GL_INT, sizeof(Vertex),
      (void *)(offsetof(Vertex, bone_ids) + 4 * sizeof(int)));

  glEnableVertexAttribArray(5);
  glVertexAttribPointer(
      5, 4, GL_FLOAT, false, sizeof(Vertex),
      (void *)(offsetof(Vertex, bone_weights) + 0 * sizeof(float)));
  glEnableVertexAttribArray(6);
  glVertexAttribPointer(
      6, 4, GL_FLOAT, false, sizeof(Vertex),
      (void *)(offsetof(Vertex, bone_weights) + 4 * sizeof(float)));

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
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

void Mesh::Draw(std::weak_ptr<Shader> shader_ptr) const {
  int tot = 0;
  for (auto kv : textures_) {
    bool enabled = kv.second.enabled;
    std::string name = SnakeToPascal(kv.first);
    shader_ptr.lock()->SetUniform<int32_t>(std::string("u") + name + "Enabled",
                                           enabled);
    if (enabled) {
      glActiveTexture(GL_TEXTURE0 + tot);
      glBindTexture(GL_TEXTURE_2D, kv.second.id);
      shader_ptr.lock()->SetUniform<int32_t>(
          std::string("u") + name + "Texture", tot);
      tot++;
    }
  }

  glBindVertexArray(vao_);
  glDrawArrays(GL_TRIANGLES, 0, indices_size_);
  glBindVertexArray(0);
}
