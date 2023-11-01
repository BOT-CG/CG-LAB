#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <functional>
class GLFWWindowFactory
{
public:
    GLFWWindowFactory(int width, int height, const char *title)
    {
        // Same window initialization logic here
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

        this->window = glfwCreateWindow(width, height, title, NULL, NULL);

        if (window == NULL)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            exit(-1);
        }

        glfwMakeContextCurrent(this->window);
        glfwSetFramebufferSizeCallback(this->window, framebuffer_size_callback);
        // glad: load all OpenGL function pointers
        // ---------------------------------------
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
        }
    }

    void processInput()
    {
        if (glfwGetKey(this->window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(this->window, true);
    }

    GLFWwindow *getWindow()
    {
        return this->window;
    }
    void run(std::function<void()> updateFunc)
    {
        while (!glfwWindowShouldClose(this->window))
        {
            // Input
            processInput();

            // Custom update function
            updateFunc();

            // Swap buffers and poll events
            glfwSwapBuffers(this->window);
            glfwPollEvents();
        }

        glfwTerminate();
    }
    static void framebuffer_size_callback(GLFWwindow *window, int width, int height)
    {
        glViewport(0, 0, width, height);
    }

private:
    GLFWwindow *window;
};
