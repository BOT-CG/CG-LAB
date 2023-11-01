#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <functional>
#include "ourcamera.h"

// #include "quaternionCamera.h"
// use quaternion Camera if you want to view how to solve gimbal lock

class GLFWWindowFactory
{
public:
    static bool blinn;
    GLFWWindowFactory(){}
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
        glfwMakeContextCurrent(window);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        glfwSetCursorPosCallback(window, mouse_callback);
        glfwSetScrollCallback(window, scroll_callback);
        // tell GLFW to capture our mouse
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        // camera



    }


    GLFWwindow *getWindow()
    {
        return this->window;
    }
    void run(std::function<void()> updateFunc)
    {
        glEnable(GL_DEPTH_TEST);

        while (!glfwWindowShouldClose(this->window))
        {
            // Input

            //clear screen
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            float currentFrame = glfwGetTime();
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            timeElapsed += deltaTime;
            frameCount++;
    
            // if (timeElapsed >= 1.0f)  // If one second has passed
            // {
            //     std::cout << "FPS: " << frameCount << std::endl;
            //     frameCount = 0;
            //     timeElapsed = 0;
            // }

            GLFWWindowFactory::processInput(window);
            // Custom update function
            projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
            view = camera.GetViewMatrix();
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

    static void processInput(GLFWwindow *window)
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera.ProcessKeyboard(FORWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera.ProcessKeyboard(BACKWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera.ProcessKeyboard(LEFT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera.ProcessKeyboard(RIGHT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
            camera.ProcessKeyboard(UP, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
            camera.ProcessKeyboard(DOWN, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
            camera.ProcessKeyboard(PITCH_UP, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
            camera.ProcessKeyboard(PITCH_DOWN, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
            camera.ProcessKeyboard(YAW_LEFT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
            camera.ProcessKeyboard(YAW_RIGHT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
            camera.ProcessKeyboard(ROLL_LEFT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
            camera.ProcessKeyboard(ROLL_RIGHT, deltaTime);



    }

    // glfw: whenever the window size changed (by OS or user resize) this callback function executes
    // ---------------------------------------------------------------------------------------------



    // glfw: whenever the mouse moves, this callback is called
    // -------------------------------------------------------
    static void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
    {
        // Check if the left mouse button is pressed
      
        float xpos = static_cast<float>(xposIn);
        float ypos = static_cast<float>(yposIn);

        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

        lastX = xpos;
        lastY = ypos;

        camera.ProcessMouseMovement(xoffset, yoffset);
    
    }


    // glfw: whenever the mouse scroll wheel scrolls, this callback is called
    // ----------------------------------------------------------------------
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
    {
        camera.ProcessMouseScroll(static_cast<float>(yoffset));
    }
    const glm::mat4 getProjectionMatrix(){
        return projection;
    }
    glm::mat4 getViewMatrix(){
        return camera.GetViewMatrix();
    }
public:
    glm::mat4 projection;
    glm::mat4 view;
    static Camera camera;
    static const unsigned int SCR_WIDTH = 800;
    static const unsigned int SCR_HEIGHT = 600;
private:
    GLFWwindow *window;

    // camera
    float timeElapsed;
    int frameCount;
    
    static float lastX;
    static float lastY;
    static bool firstMouse;

    // timing
    static float deltaTime;
    static float lastFrame;
};
