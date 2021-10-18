//
//  cg_exception.cpp
//  skinned-animation
//
//  Created by tigertang on 2018/8/3.
//  Copyright © 2018 tigertang. All rights reserved.
//

#include "cg_exception.h"

using std::string;

ShaderCompileError::ShaderCompileError(const string &title, const string &log) {
    error_message = "[shader compile error on " + title + "] " + log;
}

const char *ShaderCompileError::what() const noexcept {
    return error_message.c_str();
}

ShaderLinkError::ShaderLinkError(const string &log) {
    error_message = "[shader link error] " + log;
}

const char *ShaderLinkError::what() const noexcept {
    return error_message.c_str();
}

LoadPictureError::LoadPictureError(const std::string &path) {
    error_message = "[picture format error] Fail to load picture at " + string(path.c_str());
}

const char *LoadPictureError::what() const noexcept {
    return error_message.c_str();
}

AssimpError::AssimpError(const string &error_string) {
    error_message = "[assimp error] " + error_string;
}

const char *AssimpError::what() const noexcept {
    return error_message.c_str();
}

ShaderSettingError::ShaderSettingError(const string &name) {
    error_message = "[shader setting error] Fail to set uniform variable " + name;
}

const char *ShaderSettingError::what() const noexcept {
    return error_message.c_str();
}

MaxBoneExceededError::MaxBoneExceededError() = default;

const char *MaxBoneExceededError::what() const noexcept {
    return "[max bone exceeded error]";
}
