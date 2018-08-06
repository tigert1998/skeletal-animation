//
//  model.h
//  skinned-animation
//
//  Created by tigertang on 2018/8/2.
//  Copyright © 2018 tigertang. All rights reserved.
//

#pragma once

#include <vector>
#include <string>
#include <memory>

#include <boost/filesystem.hpp>

#include <assimp/scene.h>
#include <glm/glm.hpp>

#include "mesh.h"
#include "shader.h"
#include "camera.h"
#include "namer.h"

class SpriteModel {
    friend class Mesh;
    
public:
    SpriteModel() = delete;
    SpriteModel(boost::filesystem::path path);
    ~SpriteModel();
    void Draw(std::weak_ptr<Camera> camera_ptr);
    void Draw(std::weak_ptr<Camera> camera_ptr, double time);
    
private:
    boost::filesystem::path directory_path_;
    std::vector<std::shared_ptr<Mesh>> mesh_ptrs_;
    const aiScene *scene_;
    std::shared_ptr<Shader> shader_ptr_;
    Namer bone_namer_;
    std::vector<glm::mat4> bone_matrices_;
    void RecursivelyInitNodes(aiNode *node, glm::mat4 transform);
    
    
};

