#pragma once

#include <Scene/Types.h>

#include <glm/gtc/matrix_transform.hpp>

struct V8_Camera {
  Vector3 position;
  Vector3 front;
  Vector3 up;
  Vector3 right;
  Vector3 worldUp;
  float yaw;
  float pitch;
  float movementSpeed;
  float mouseSensitivity;
  float zoom;

  Matrix4 GetViewMatrix() {
    Matrix4 view;
    view = glm::lookAt(position, position + front, up);
    return view;
  }
};
