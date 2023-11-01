#include "windowFactory.h"
#include "utils/RectangleModel.h"

int main()
{
    GLFWWindowFactory myWindow(800, 600, "LearnOpenGL");
    Shader shader("shader.vs", "shader.fs");
    RectangleModel rectangle(shader);

    myWindow.run([&]()
                 {
                    shader.setMat4("projection", myWindow.getProjectionMatrix());
                    shader.setMat4("view", myWindow.getViewMatrix());
                    rectangle.draw();
                     // Custom model updates and rendering logic here.
                     // For example:
                     // model->update();
                     // renderer->draw(model);
                 });

    return 0;
}
