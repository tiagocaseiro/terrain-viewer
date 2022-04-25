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
#include "editor.hpp"
#include "mouse.hpp"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// settings
static constexpr auto SCR_WIDTH  = 800;
static constexpr auto SCR_HEIGHT = 600;

static auto mouseMovementCallbacks = std::vector<std::function<void(float, float)>>();
static auto mouseScrollCallbacks   = std::vector<std::function<void(float, float)>>();
static auto mouseButtonCallbacks   = std::vector<std::function<void(int, int, int)>>();

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

Mouse::StateMachine mouse_state;


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
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);

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
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // draw in wireframe
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    Camera camera({0.0f, 3.0f, 10.0f}, {0.0f, 1.0f, 0.0f}, -90.f, -20.0f);
    Shader cursor_shader("shaders/cursor.vs", "shaders/cursor.fs");
    Shader triangle_shader("shaders/triangle.vs", "shaders/default.fs");
    Shader wireframe_shader("shaders/wireframe.vs", "shaders/default.fs", "shaders/wireframe.gs");
    Editor editor(10, 10, Cursor{0.03, {0.79f, 0.071f, 0.13f}, 0.5f});
    mouse_state.add(Mouse::State::Default, Mouse::Action::LeftPress, Mouse::State::LeftPressed);
    mouse_state.add(
        Mouse::State::LeftPressed, Mouse::Action::LeftRelease, Mouse::State::Default, [&editor] {
            editor.reset();
        });
    mouse_state.add(
        Mouse::State::LeftPressed, Mouse::Action::ScrollUp, Mouse::State::LeftPressed, [&editor] {
            editor.increment(0.01f);
            editor.set();
        });
    mouse_state.add(
        Mouse::State::LeftPressed, Mouse::Action::ScrollDown, Mouse::State::LeftPressed, [&editor] {
            editor.increment(-0.01f);
            editor.set();
        });
    mouse_state.add<float, float>(
        Mouse::State::LeftPressed,
        Mouse::Action::Movement,
        Mouse::State::LeftPressed,
        [&editor, &camera](auto xoffset, auto zoffset) {
            editor.updateCursor(xoffset, zoffset);
            editor.set();
        });
    mouse_state.add<float, float>(
        Mouse::State::Default,
        Mouse::Action::Movement,
        Mouse::State::Default,
        [&editor, &camera](auto xoffset, auto zoffset) { editor.updateCursor(xoffset, zoffset); });

    auto projection = glm::perspective(
        glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    // auto projection = glm::ortho(-4.f, 4.f, -3.f, 3.f, 0.1f, 100.0f);

    cursor_shader.use();
    cursor_shader.set("projection", projection);
    cursor_shader.set("view", camera.GetViewMatrix());

    triangle_shader.use();
    triangle_shader.set("projection", projection);
    triangle_shader.set("view", camera.GetViewMatrix());

    wireframe_shader.use();
    wireframe_shader.set("projection", projection);
    wireframe_shader.set("view", camera.GetViewMatrix());

    mouseMovementCallbacks.push_back([firstMouse = true,
                                      lastX      = SCR_WIDTH / 2.0f,
                                      lastY = SCR_HEIGHT / 2.0f](auto xpos, auto ypos) mutable {
        if (firstMouse) {
            lastX      = xpos;
            lastY      = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

        lastX = xpos;
        lastY = ypos;

        mouse_state.execute(Mouse::Action::Movement, xoffset, yoffset);
    });

    mouseButtonCallbacks.push_back([](auto button, auto action, auto) {
        static std::map<int, std::map<int, Mouse::Action>> config{
            {GLFW_MOUSE_BUTTON_LEFT,
             {{GLFW_PRESS, Mouse::Action::LeftPress}, {GLFW_RELEASE, Mouse::Action::LeftRelease}}},
            {GLFW_MOUSE_BUTTON_RIGHT,
             {{GLFW_PRESS, Mouse::Action::RightPress},
              {GLFW_RELEASE, Mouse::Action::RightRelease}}}};
        mouse_state.execute(config[button][action]);
    });
    mouseScrollCallbacks.push_back([](auto xoffset, auto yoffset) {
        if (yoffset > 0.0f)
            mouse_state.execute(Mouse::Action::ScrollUp);
        else if (yoffset < 0.0f)
            mouse_state.execute(Mouse::Action::ScrollDown);
    });

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
        glClearColor(0.0f, 0.0f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        editor.draw(triangle_shader, wireframe_shader, cursor_shader);

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
    for (auto& callback : mouseMovementCallbacks)
        callback(xpos, ypos);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    for (auto& callback : mouseButtonCallbacks)
        callback(button, action, mods);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    for (auto& callback : mouseScrollCallbacks)
        callback(xoffset, yoffset);
}
