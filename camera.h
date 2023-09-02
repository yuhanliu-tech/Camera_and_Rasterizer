#pragma once

#ifndef CAMERA_H
#define CAMERA_H
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

// Create a Camera class that you will use to generate view matrices and
// perspective projection matrices.

class Camera
{

protected:

    float fov;

    glm::vec4 worldPos;

    float nearClipPlane;
    float farClipPlane;
    float cameraRatio;

    glm::vec4 forward;
    glm::vec4 right;
    glm::vec4 up;

public:

    Camera();
    glm::mat4 returnViewMat();
    glm::mat4 returnPersProjMat();

    void translateX(float x);
    void translateY(float y);
    void translateZ(float z);

    void rotateX(float x);
    void rotateY(float y);
    void rotateZ(float z);

    glm::vec4 getForward();

};

#endif // CAMERA_H
