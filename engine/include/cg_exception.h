//
//  cg_exception.h
//  skeletal-animation
//
//  Created by tigertang on 2018/8/3.
//  Copyright © 2018 tigertang. All rights reserved.
//

#pragma once

#include <string>

class ShaderCompileError: public std::exception {
public:
    ShaderCompileError() = delete;
    ShaderCompileError(const std::string &title, const std::string &log);
    const char *what() const noexcept;
    
private:
    std::string error_message;
};

class ShaderLinkError: public std::exception {
public:
    ShaderLinkError() = delete;
    ShaderLinkError(const std::string &log);
    const char *what() const noexcept;
    
private:
    std::string error_message;
};

class LoadPictureError: public std::exception {
public:
    LoadPictureError() = delete;
    LoadPictureError(const std::string &path);
    const char *what() const noexcept;
    
private:
    std::string error_message;
};

class AssimpError: public std::exception {
public:
    AssimpError() = delete;
    AssimpError(const std::string &error_string);
    const char *what() const noexcept;
    
private:
    std::string error_message;
};

class MaxBoneExceededError: public std::exception {
public:
    MaxBoneExceededError();
    const char *what() const noexcept;
};

class ShaderSettingError: public std::exception {
public:
    ShaderSettingError() = delete;
    ShaderSettingError(const std::string &name);
    const char *what() const noexcept;
    
private:
    std::string error_message;
};
