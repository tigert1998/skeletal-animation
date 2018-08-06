//
//  mesh.cpp
//  skinned-animation
//
//  Created by tigertang on 2018/8/3.
//  Copyright © 2018 tigertang. All rights reserved.
//

#include <algorithm>

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

#include "mesh.h"
#include "texture_manager.h"

using namespace glm;
using namespace boost;

Mesh::Mesh(filesystem::path directory_path, aiMesh *mesh, const aiScene *scene) {
    static std::vector<Vertex> vertices;
    static std::vector<uint32_t> indices;
    vertices.clear();
    indices.clear();
    
    static float max_depth = 0;
    
    auto path = directory_path;
    {
        aiString material_texture_path;
        auto material = scene->mMaterials[mesh->mMaterialIndex];
        material->GetTexture(aiTextureType_DIFFUSE, 0, &material_texture_path);
        std::string item = material_texture_path.C_Str();
        int i;
        for (i = (int) item.length() - 1; i >= 0; i--) if (item[i] == '\\') break;
        item = item.substr(i + 1);
        path /= item;
    }
    texture_id_ = TextureManager::LoadTexture(path);
    for (int i = 0; i < mesh->mNumVertices; i++) {
        auto vertex = Vertex();
        vertex.position = vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        vertex.tex_coord = vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        vertex.normal = vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        vertices.push_back(vertex);
        
        max_depth = max(max_depth, vertex.position.z);
    }
    for (int i = 0; i < mesh->mNumFaces; i++) {
        auto face = mesh->mFaces[i];
        for (int j = 0; j < face.mNumIndices; j++) indices.push_back(face.mIndices[j]);
    }
    indices_size_ = (uint32_t) indices.size();
        
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glGenBuffers(1, &ebo_);
    
    glBindVertexArray(vao_);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * indices.size(), indices.data(), GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex), (void *) offsetof(Vertex, position));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(Vertex), (void *) offsetof(Vertex, tex_coord));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, false, sizeof(Vertex), (void *) offsetof(Vertex, normal));
    
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

Mesh::~Mesh() {
//    glDeleteVertexArrays(1, &vao_);
//    glDeleteBuffers(1, &vbo_);
//    glDeleteBuffers(1, &ebo_);
//    glDeleteTextures(1, &texture_id_);
}

void Mesh::Draw(std::weak_ptr<Camera> camera_ptr, std::weak_ptr<Shader> shader_ptr) const {
    auto shader_shared_ptr = shader_ptr.lock();
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_id_);
    shader_shared_ptr->SetUniform<int32_t>("uDiffuseTexture", 0);
    
    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, indices_size_, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
