//
//  vertex.cpp
//  skinned-animation
//
//  Created by tigertang on 2018/8/2.
//  Copyright © 2018 tigertang. All rights reserved.
//


#include <algorithm>

#include "vertex.h"
#include "cg_exception.h"

Vertex::Vertex() {
    std::fill(bone_ids, bone_ids + kMaxBonesPerVertex, 0);
    std::fill(bone_weights, bone_weights + kMaxBonesPerVertex, 0);
}

void Vertex::AddBone(int id, float weight) {
    int i;
    for (i = 0; i < kMaxBonesPerVertex; i++) if (bone_weights[i] == 0) break;
    if (i >= kMaxBonesPerVertex) throw MaxBoneExceededError();
    bone_weights[i] = weight;
    bone_ids[i] = id;
}
