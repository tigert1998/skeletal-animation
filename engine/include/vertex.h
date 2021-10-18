//
//  vertex.h
//  skinned-animation
//
//  Created by tigertang on 2018/8/2.
//  Copyright © 2018 tigertang. All rights reserved.
//

#pragma once

#include <stdint.h>

#include <glm/glm.hpp>

constexpr int kMaxBonesPerVertex = 8;

class Vertex {
public:
    glm::vec3 position;
    glm::vec2 tex_coord;
    glm::vec3 normal;
    int bone_ids[kMaxBonesPerVertex];
    float bone_weights[kMaxBonesPerVertex];
    
    Vertex();
    void AddBone(int id, float weight);
};
