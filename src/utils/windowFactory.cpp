#include "windowFactory.h"

// Outside of any class definition
Camera GLFWWindowFactory::camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));

float GLFWWindowFactory::lastX = GLFWWindowFactory::SCR_WIDTH / 2.0f;
float GLFWWindowFactory::lastY = GLFWWindowFactory::SCR_HEIGHT / 2.0f;
bool GLFWWindowFactory::firstMouse = true;
float GLFWWindowFactory::deltaTime = 0.0f;
float GLFWWindowFactory::lastFrame = 0.0f;
bool GLFWWindowFactory::blinn = false;  // Or true, depending on your needs
