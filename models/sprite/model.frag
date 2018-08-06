#version 400 core

const float zero = 0.00000001;

in vec3 vPosition;
in vec2 vTexCoord;
in vec3 vNormal;

uniform sampler2D uDiffuseTexture;

out vec4 fragColor;

void main() {
    fragColor = texture(uDiffuseTexture, vTexCoord);
}
