//
//  wall.h
//  skinned-animation
//
//  Created by tigertang on 2018/8/5.
//  Copyright © 2018 tigertang. All rights reserved.
//

#pragma once

#include <stdint.h>

#include "camera.h"
#include "shader.h"

class Wall {
private:
    uint32_t vao_, vbo_;
    std::shared_ptr<Shader> shader_ptr_;
    
public:
    Wall();
    ~Wall();
    void Draw(std::weak_ptr<Camera> camera_ptr) const;
};
