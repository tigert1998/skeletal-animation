#version 410 core

const int MAX_BONES = 100;

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in ivec4 aBoneIDs0;
layout (location = 4) in ivec4 aBoneIDs1;
layout (location = 5) in vec4 aBoneWeights0;
layout (location = 6) in vec4 aBoneWeights1;

out vec3 vPosition;
out vec2 vTexCoord;
out vec3 vNormal;

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;
uniform mat4 uBoneMatrices[MAX_BONES];

mat4 CalcBoneMatrix() {
    mat4 boneMatrix = mat4(0);
    for (int i = 0; i < 4; i++) {
        boneMatrix += uBoneMatrices[aBoneIDs0[i]] * aBoneWeights0[i];
        boneMatrix += uBoneMatrices[aBoneIDs1[i]] * aBoneWeights1[i];
    }
    return boneMatrix;
}

void main() {
    mat4 boneMatrix = CalcBoneMatrix();
    gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * boneMatrix * vec4(aPosition, 1);
    vPosition = vec3(uModelMatrix * boneMatrix * vec4(aPosition, 1));
    vTexCoord = aTexCoord;
    vNormal = vec3(uModelMatrix * boneMatrix * vec4(aPosition, 0));
}
