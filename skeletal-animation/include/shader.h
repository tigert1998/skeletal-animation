//
//  shader.h
//  skinned-animation
//
//  Created by tigertang on 2018/8/3.
//  Copyright © 2018 tigertang. All rights reserved.
//

#pragma once

#include <string>
class Shader {
public:
    enum {
        SRC,
        PATH
    };

    Shader() = delete;
    Shader(uint32_t type, const std::string &vs, const std::string &fs);
    void Use() const;
    template <typename T> void SetUniform(const std::string &identifier, const T&) const;
    
private:
    static uint32_t Compile(uint32_t type, const std::string &source, const std::string &path);
    static uint32_t Link(uint32_t vs_id, uint32_t fs_id);
    
    uint32_t id;
};
