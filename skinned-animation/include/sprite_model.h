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

class SpriteModel {
public:
    SpriteModel() = delete;
    SpriteModel(boost::filesystem::path path);
    ~SpriteModel();
    void Draw(std::weak_ptr<Camera> camera_ptr) const;
    
private:
    boost::filesystem::path directory_path_;
    std::vector<Mesh> meshes_;
    const aiScene *scene_;
    std::shared_ptr<Shader> shader_ptr_;
    void RecursivelyInitNodes(aiNode *node, glm::mat4 transform);
    
};

