#include "utils/Cube.h"

int main()
{
    GLFWWindowFactory myWindow(800, 600, "LearnOpenGL");
    Cube cube(&myWindow);

    myWindow.run([&]()
                 {
                    cube.draw();

                     // Custom model updates and rendering logic here.
                     // For example:
                     // model->update();
                     // renderer->draw(model);
                 });

    return 0;
}
