#pragma once

#include <Renderer/Utils.h>

struct V8_TransformUBO : V8_UBOBase {
  glm::mat4 model_ = glm::mat4(1.0f);
  glm::mat4 view_ = glm::mat4(1.0f);
  glm::mat4 projection_ = glm::mat4(1.0f);
};
