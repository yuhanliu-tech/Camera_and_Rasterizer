#include "camera.h"

Camera::Camera()
    : forward(0,0,-1,0), right(1,0,0,0), up(0,1,0,0), fov(45), worldPos(0,0,10,1), nearClipPlane(0.01), farClipPlane(100.0), cameraRatio(1.0)
{}

glm::mat4 Camera::returnViewMat() {
    // uses forward, right, up, worldpos
    // don't forget the 1 at the bottom right

    glm::mat4 o = glm::mat4(
                glm::vec4(right[0], up[0], forward[0], 0),
                glm::vec4(right[1], up[1], forward[1], 0),
                glm::vec4(right[2], up[2], forward[2], 0),
                glm::vec4(0,0,0,1));

    glm::mat4 t = glm::mat4(glm::vec4(1,0,0,0), glm::vec4(0,1,0,0), glm::vec4(0,0,1,0),
                            glm::vec4(-1 * worldPos[0], -1 * worldPos[1], -1 * worldPos[2], 1));

    return o * t;
}

glm::mat4 Camera::returnPersProjMat() {
    // uses aspect ratio, fov, near/far

    return glm::mat4(
                glm::vec4( 1 / (cameraRatio * glm::tan(glm::radians(fov / 2))), 0, 0, 0),
                glm::vec4( 0, 1 / (glm::tan(glm::radians(fov / 2))), 0, 0),
                glm::vec4(0, 0, farClipPlane / (farClipPlane - nearClipPlane), 1),
                glm::vec4(0,0, (-1 * farClipPlane * nearClipPlane) / (farClipPlane - nearClipPlane),0));

}

void Camera::translateX(float x) {
    worldPos += (right * x);
}

void Camera::translateY(float y) {
    worldPos[1] += y;
}

void Camera::translateZ(float z) {
    worldPos[2] += z;
}

void Camera::rotateX(float x) {
    // don't edit x-axis (right)
    // just up and forward

    forward = glm::rotate(glm::mat4(), x, glm::vec3(right)) * forward;
    up = glm::rotate(glm::mat4(), x,  glm::vec3(right)) * up;
}

void Camera::rotateY(float y) {
    // glm rotate(angle) + vector rotating along (y-axis)

    right = glm::rotate(glm::mat4(), y, glm::vec3(up)) * right;
    forward = glm::rotate(glm::mat4(), y, glm::vec3(up)) * forward;

}

void Camera::rotateZ(float z) {

    up = glm::rotate(glm::mat4(), z, glm::vec3(forward)) * up;
    right = glm::rotate(glm::mat4(), z, glm::vec3(forward)) * right;

}

glm::vec4 Camera::getForward() {
    return -forward;
}
