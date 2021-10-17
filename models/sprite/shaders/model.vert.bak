#version 400 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

out vec3 vPosition;
out vec2 vTexCoord;
out vec3 vNormal;

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;

void main() {
    gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * vec4(aPosition, 1);
    vPosition = vec3(uModelMatrix * vec4(aPosition, 1));
    vTexCoord = aTexCoord;
    vNormal = vec3(uModelMatrix * vec4(aPosition, 0));
}
