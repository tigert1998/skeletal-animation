//
//  vertex.cpp
//  skinned-animation
//
//  Created by tigertang on 2018/8/2.
//  Copyright © 2018 tigertang. All rights reserved.
//

#include "vertex.h"

#include <algorithm>

#include "cg_exception.h"

Vertex::Vertex() {
  std::fill(bone_ids, bone_ids + kMaxBonesPerVertex, -1);
  std::fill(bone_weights, bone_weights + kMaxBonesPerVertex, 0);
}

void Vertex::AddBone(int id, float weight) {
  int i = NumBones();
  if (i >= kMaxBonesPerVertex) throw MaxBoneExceededError();
  bone_weights[i] = weight;
  bone_ids[i] = id;
}

int Vertex::NumBones() {
  for (int i = 0; i < kMaxBonesPerVertex; i++)
    if (bone_ids[i] < 0) return i;
  return kMaxBonesPerVertex;
}
