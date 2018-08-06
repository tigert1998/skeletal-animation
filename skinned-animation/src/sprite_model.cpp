//
//  model.cpp
//  skinned-animation
//
//  Created by tigertang on 2018/8/2.
//  Copyright © 2018 tigertang. All rights reserved.
//

#include <glad/glad.h>
#include <assimp/cimport.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <glm/gtc/matrix_transform.hpp>

#include "sprite_model.h"

using std::string;
using std::shared_ptr;
using namespace Assimp;
using namespace glm;

SpriteModel::SpriteModel(boost::filesystem::path path): directory_path_(path.parent_path()) {
    scene_ = aiImportFile(path.c_str(), aiProcess_CalcTangentSpace | aiProcess_FlipUVs | aiProcess_Triangulate);
    shader_ptr_ = shared_ptr<Shader>(new Shader(directory_path_ / "model.vert", directory_path_ / "model.frag"));
    RecursivelyInitNodes(scene_->mRootNode, mat4(1));
}

SpriteModel::~SpriteModel() {
    aiReleaseImport(scene_);
}

void SpriteModel::RecursivelyInitNodes(aiNode *node, mat4 transform) {
    if (node->mName != aiString("objTwoHand13_SM") && node->mName != aiString("Plane001") && node->mName != aiString("Plane002")) {
        for (int i = 0; i < node->mNumMeshes; i++) {
            auto mesh = scene_->mMeshes[node->mMeshes[i]];
            meshes_.emplace_back(directory_path_, mesh, scene_);
        }
    }
    for (int i = 0; i < node->mNumChildren; i++) {
        RecursivelyInitNodes(node->mChildren[i], transform);
    }
}

void SpriteModel::Draw(std::weak_ptr<Camera> camera_ptr) const {
    shader_ptr_->Use();
    shader_ptr_->SetUniform<mat4>("uModelMatrix", rotate(mat4(1), -pi<float>() / 2.0f, vec3(1, 0, 0)));
    shader_ptr_->SetUniform<mat4>("uViewMatrix", camera_ptr.lock()->view_matrix());
    shader_ptr_->SetUniform<mat4>("uProjectionMatrix", camera_ptr.lock()->projection_matrix());
    
    for (const auto &mesh: meshes_) {
        mesh.Draw(camera_ptr, shader_ptr_);
    }
}
