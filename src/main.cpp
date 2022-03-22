#define STB_IMAGE_IMPLEMENTATION

#include <functional>
#include <iostream>

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "learnopengl/camera.hpp"
#include "learnopengl/shader.hpp"

#include "circle.hpp"
#include "cursor.hpp"
#include "grid.hpp"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow* window);

// settings
static constexpr auto SCR_WIDTH  = 800;
static constexpr auto SCR_HEIGHT = 600;

static auto mouseCallbacks = std::vector<std::function<void(float, float)>>();

float lastX     = SCR_WIDTH / 2.0f;
float lastY     = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main(int argv, char* argc[]) {

    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    auto window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Game", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    // stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // draw in wireframe
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    Camera camera({0.0f, 3.0f, 10.0f}, {0.0f, 1.0f, 0.0f}, -90.f, -20.0f);
    Cursor cursor(Circle{50, 0.5}, 0.03, {0.79f, 0.071f, 0.13f});
    Grid grid(800, 600);
    Shader shader("shaders/default.vs", "shaders/default.fs");

    auto projection = glm::perspective(
        glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    // auto projection = glm::ortho(-4.f, 4.f, -3.f, 3.f, 0.1f, 100.0f);

    shader.use();
    shader.set("projection", projection);
    shader.set("view", camera.GetViewMatrix());

    mouseCallbacks.push_back(
        [&camera](auto xoffset, auto yoffset) { camera.ProcessMouseMovement(xoffset, yoffset); });
    mouseCallbacks.push_back(
        [&cursor](auto xoffset, auto yoffset) { cursor.updatePosition(xoffset, yoffset); });

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        // per-frame time logic
        // --------------------
        auto currentFrame = glfwGetTime();
        deltaTime         = currentFrame - lastFrame;
        lastFrame         = currentFrame;

        processInput(window);
        // render
        // ------
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.set("color", glm::vec3{1.0, 1.0, 1.0});
        shader.set("model", glm::mat4(1.0f));
        grid.draw(shader);

        glDepthFunc(GL_ALWAYS);
        auto model = glm::mat4(1.0f);
        model      = glm::translate(model, cursor.getPosition());
        model      = glm::rotate(model, glm::radians(90.f), {1.0f, 0.0f, 0.0f});
        shader.set("model", model);
        shader.set("color", cursor.getColor());
        cursor.draw(shader);
        glDepthFunc(GL_LESS);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react
// accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX      = xpos;
        lastY      = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    for (auto& callback : mouseCallbacks)
        callback(xoffset, yoffset);
}
