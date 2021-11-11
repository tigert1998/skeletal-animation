//
//  vertex.h
//  skeletal-animation
//
//  Created by tigertang on 2018/8/2.
//  Copyright © 2018 tigertang. All rights reserved.
//

#pragma once

#include <stdint.h>

#include <glm/glm.hpp>

#include "cg_exception.h"

template <int MaxBonesPerVertex>
class VertexWithBonesBase {
 public:
  VertexWithBonesBase();
  void AddBone(int id, float weight);
  int NumBones();
};

template <int MaxBonesPerVertex>
VertexWithBonesBase<MaxBonesPerVertex>::VertexWithBonesBase() {
  std::fill(bone_ids, bone_ids + MaxBonesPerVertex, -1);
  std::fill(bone_weights, bone_weights + MaxBonesPerVertex, 0);
}

template <int MaxBonesPerVertex>
void VertexWithBonesBase<MaxBonesPerVertex>::AddBone(int id, float weight) {
  int i = NumBones();
  if (i >= MaxBonesPerVertex) throw MaxBoneExceededError();
  bone_weights[i] = weight;
  bone_ids[i] = id;
}

template <int MaxBonesPerVertex>
int VertexWithBonesBase<MaxBonesPerVertex>::NumBones() {
  for (int i = 0; i < MaxBonesPerVertex; i++)
    if (bone_ids[i] < 0) return i;
  return MaxBonesPerVertex;
}

template <int MaxBonesPerVertex, bool HasTexCoord = true>
class Vertex : public VertexWithBonesBase<MaxBonesPerVertex> {
 public:
  glm::vec3 position;
  glm::vec2 tex_coord;
  glm::vec3 normal;
};

template <int MaxBonesPerVertex>
class Vertex<MaxBonesPerVertex, false>
    : public VertexWithBonesBase<MaxBonesPerVertex> {
 public:
  glm::vec3 position;
  glm::vec3 normal;
};

template <>
class Vertex<0, true> {
 public:
  glm::vec3 position;
  glm::vec2 tex_coord;
  glm::vec3 normal;
};

template <>
class Vertex<0, false> {
 public:
  glm::vec3 position;
  glm::vec3 normal;
};
