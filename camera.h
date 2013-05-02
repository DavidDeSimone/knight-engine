#ifndef CAMERA_H_
#define CAMERA_H_

#include "utils.h"

#include <glm/glm.hpp>

class Camera {
 public:
  glm::mat4 projection_matrix_;

  Camera();
  virtual ~Camera();

 private:
  DISALLOW_COPY_AND_ASSIGN(Camera);
};

#endif // CAMERA_H_
