#include "utils.h"

glm::mat4 Mat4FromAimatrix4x4(aiMatrix4x4 matrix) {
    glm::mat4 res;
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++) res[j][i] = matrix[i][j];
    return res;
};