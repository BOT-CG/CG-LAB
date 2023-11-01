#include "windowFactory.h"
#include "utils/TriangleModel.h"

int main()
{
    GLFWWindowFactory myWindow(800, 600, "LearnOpenGL");

    TriangleModel triangle;

    myWindow.run([&]()
                 {
                     triangle.draw();
                     // Custom model updates and rendering logic here.
                     // For example:
                     // model->update();
                     // renderer->draw(model);
                 });

    return 0;
}
