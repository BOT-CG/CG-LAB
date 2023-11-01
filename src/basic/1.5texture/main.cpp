#include "windowFactory.h"
#include "utils/RectangleModel.h"

int main()
{
    GLFWWindowFactory myWindow(800, 600, "LearnOpenGL");
    Shader shader = Shader("shader.vs", "shader.fs");
    RectangleModel rectangle(shader);

    myWindow.run([&]()
                 {
                     rectangle.draw();
                     // Custom model updates and rendering logic here.
                     // For example:
                     // model->update();
                     // renderer->draw(model);
                 });

    return 0;
}
