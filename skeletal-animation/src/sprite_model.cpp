//
//  model.cpp
//  skinned-animation
//
//  Created by tigertang on 2018/8/2.
//  Copyright © 2018 tigertang. All rights reserved.
//

#include <iostream>

#include <glad/glad.h>
#include <assimp/cimport.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <glm/gtc/matrix_transform.hpp>

#include "sprite_model.h"
#include "utils.h"

using std::string;
using std::shared_ptr;
using std::make_shared;
using std::vector;
using std::pair;
using namespace Assimp;
using namespace glm;

namespace {
    std::string ParentPath(const std::string &path) {
        int i;
        for (i = path.size() - 2; i >= 0; i--) {
            if (path[i] == '/' || path[i] == '\\' && path[i + 1] == '\\') {
                break;   
            }
        }
        return path.substr(0, i);
    }
}

SpriteModel::SpriteModel(const std::string &path): directory_path_(ParentPath(path)) {
    scene_ = aiImportFile(path.c_str(), aiProcess_CalcTangentSpace | aiProcess_FlipUVs | aiProcess_Triangulate);
    shader_ptr_ = shared_ptr<Shader>(new Shader(Shader::PATH, directory_path_ + "/model.vert", directory_path_ + "/model.frag"));

    animation_channel_map_.clear();
    for (int i = 0; i < scene_->mNumAnimations; i++) {
        auto animation = scene_->mAnimations[i];
        for (int j = 0; j < animation->mNumChannels; j++) {
            auto channel = animation->mChannels[j];
            animation_channel_map_[pair<uint32_t, string>(i, channel->mNodeName.C_Str())] = j;
        }
    }
    
    RecursivelyInitNodes(scene_->mRootNode);
    bone_matrices_.resize(bone_namer_.total());
}

SpriteModel::~SpriteModel() {
    aiReleaseImport(scene_);
}

void SpriteModel::RecursivelyInitNodes(aiNode *node) {
    if (node->mName != aiString("objTwoHand13_SM") && node->mName != aiString("Plane001") && node->mName != aiString("Plane002") && node->mName != aiString("obj53002_LynM001")) {
        for (int i = 0; i < node->mNumMeshes; i++) {
            auto mesh = scene_->mMeshes[node->mMeshes[i]];
            mesh_ptrs_.emplace_back(make_shared<Mesh>(directory_path_, mesh, scene_, bone_namer_, bone_offsets_));
        }
    }
    for (int i = 0; i < node->mNumChildren; i++) {
        RecursivelyInitNodes(node->mChildren[i]);
    }
}

glm::mat4 SpriteModel::InterpolateTranslationMatrix(aiVectorKey *keys, uint32_t n, double ticks) {
    static auto mat4_from_aivector3d = [] (aiVector3D vector) -> mat4 {
        return translate(mat4(1), vec3(vector.x, vector.y, vector.z));
    };
    if (n == 0) return mat4(1);
    if (n == 1) return mat4_from_aivector3d(keys->mValue);
    if (ticks <= keys[0].mTime) return mat4_from_aivector3d(keys[0].mValue);
    if (keys[n - 1].mTime <= ticks) return mat4_from_aivector3d(keys[n - 1].mValue);
    
    aiVectorKey anchor;
    anchor.mTime = ticks;
    auto right_ptr = std::upper_bound(keys, keys + n, anchor, [] (const aiVectorKey &a, const aiVectorKey &b) {
        return a.mTime < b.mTime;
    });
    auto left_ptr = right_ptr - 1;
    
    float factor = (ticks - left_ptr->mTime) / (right_ptr->mTime - left_ptr->mTime);
    return mat4_from_aivector3d(left_ptr->mValue * (1.0f - factor) + right_ptr->mValue * factor);
}

glm::mat4 SpriteModel::InterpolateRotationMatrix(aiQuatKey *keys, uint32_t n, double ticks) {
    static auto mat4_from_aiquaternion = [] (aiQuaternion quaternion) -> mat4 {
        auto rotation_matrix = quaternion.GetMatrix();
        mat4 res(1);
        for (int i = 0; i < 3; i++) for (int j = 0; j < 3; j++) res[j][i] = rotation_matrix[i][j];
        return res;
    };
    if (n == 0) return mat4(1);
    if (n == 1) return mat4_from_aiquaternion(keys->mValue);
    if (ticks <= keys[0].mTime) return mat4_from_aiquaternion(keys[0].mValue);
    if (keys[n - 1].mTime <= ticks) return mat4_from_aiquaternion(keys[n - 1].mValue);
    
    aiQuatKey anchor;
    anchor.mTime = ticks;
    auto right_ptr = std::upper_bound(keys, keys + n, anchor, [] (const aiQuatKey &a, const aiQuatKey &b) {
        return a.mTime < b.mTime;
    });
    auto left_ptr = right_ptr - 1;
    
    double factor = (ticks - left_ptr->mTime) / (right_ptr->mTime - left_ptr->mTime);
    aiQuaternion out;
    aiQuaternion::Interpolate(out, left_ptr->mValue, right_ptr->mValue, factor);
    return mat4_from_aiquaternion(out);
}

glm::mat4 SpriteModel::InterpolateScalingMatrix(aiVectorKey *keys, uint32_t n, double ticks) {
    static auto mat4_from_aivector3d = [] (aiVector3D vector) -> mat4 {
        return scale(mat4(1), vec3(vector.x, vector.y, vector.z));
    };
    if (n == 0) return mat4(1);
    if (n == 1) return mat4_from_aivector3d(keys->mValue);
    if (ticks <= keys[0].mTime) return mat4_from_aivector3d(keys[0].mValue);
    if (keys[n - 1].mTime <= ticks) return mat4_from_aivector3d(keys[n - 1].mValue);
    
    aiVectorKey anchor;
    anchor.mTime = ticks;
    auto right_ptr = std::upper_bound(keys, keys + n, anchor, [] (const aiVectorKey &a, const aiVectorKey &b) {
        return a.mTime < b.mTime;
    });
    auto left_ptr = right_ptr - 1;
    
    float factor = (ticks - left_ptr->mTime) / (right_ptr->mTime - left_ptr->mTime);
    return mat4_from_aivector3d(left_ptr->mValue * (1.0f - factor) + right_ptr->mValue * factor);
}

void SpriteModel::RecursivelyUpdateBoneMatrices(int animation_id, aiNode *node, glm::mat4 transform, double ticks) {
    string node_name = node->mName.C_Str();
    auto animation = scene_->mAnimations[animation_id];
    mat4 current_transform;
    if (animation_channel_map_.count(pair<uint32_t, string>(animation_id, node_name))) {
        uint32_t channel_id = animation_channel_map_[pair<uint32_t, string>(animation_id, node_name)];
        auto channel = animation->mChannels[channel_id];
    
        // translation matrix
        mat4 translation_matrix = InterpolateTranslationMatrix(channel->mPositionKeys, channel->mNumPositionKeys, ticks);
        // rotation matrix
        mat4 rotation_matrix = InterpolateRotationMatrix(channel->mRotationKeys, channel->mNumRotationKeys, ticks);
        // scaling matrix
        mat4 scaling_matrix = InterpolateScalingMatrix(channel->mScalingKeys, channel->mNumScalingKeys, ticks);
        
        current_transform = translation_matrix * rotation_matrix * scaling_matrix;
    } else {
        current_transform = Mat4FromAimatrix4x4(node->mTransformation);
    }
    if (bone_namer_.map().count(node_name)) {
        uint32_t i = bone_namer_.map()[node_name];
        bone_matrices_[i] = transform * current_transform * bone_offsets_[i];
    }
    for (int i = 0; i < node->mNumChildren; i++) {
        RecursivelyUpdateBoneMatrices(animation_id, node->mChildren[i], transform * current_transform, ticks);
    }
}

void SpriteModel::Draw(uint32_t animation_id, std::weak_ptr<Camera> camera_ptr, double time) {
    RecursivelyUpdateBoneMatrices(animation_id, scene_->mRootNode, mat4(1), time * scene_->mAnimations[animation_id]->mTicksPerSecond);
    shader_ptr_->Use();
    shader_ptr_->SetUniform<mat4>("uModelMatrix", mat4(1));
    shader_ptr_->SetUniform<mat4>("uViewMatrix", camera_ptr.lock()->view_matrix());
    shader_ptr_->SetUniform<mat4>("uProjectionMatrix", camera_ptr.lock()->projection_matrix());
    shader_ptr_->SetUniform<vector<mat4>>("uBoneMatrices", bone_matrices_);
    
    for (const auto &mesh_ptr: mesh_ptrs_) {
        mesh_ptr->Draw(shader_ptr_);
    }
}

void SpriteModel::Draw(std::weak_ptr<Camera> camera_ptr) {
    std::fill(bone_matrices_.begin(), bone_matrices_.end(), mat4(1));
    shader_ptr_->Use();
    shader_ptr_->SetUniform<mat4>("uModelMatrix", rotate(mat4(1), -pi<float>() / 2.0f, vec3(1, 0, 0)));
    shader_ptr_->SetUniform<mat4>("uViewMatrix", camera_ptr.lock()->view_matrix());
    shader_ptr_->SetUniform<mat4>("uProjectionMatrix", camera_ptr.lock()->projection_matrix());
    shader_ptr_->SetUniform<vector<mat4>>("uBoneMatrices", bone_matrices_);
    
    for (const auto &mesh_ptr: mesh_ptrs_) {
        mesh_ptr->Draw(shader_ptr_);
    }
}
