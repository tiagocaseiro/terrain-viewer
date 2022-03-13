#define STB_IMAGE_IMPLEMENTATION

#include <iostream>

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "learnopengl/camera.hpp"
#include "learnopengl/shader.hpp"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// settings
static constexpr auto SCR_WIDTH  = 800;
static constexpr auto SCR_HEIGHT = 600;

static constexpr auto HEIGHT_MAP_WIDTH    = 800;
static constexpr auto HEIGHT_MAP_HEIGHT   = 600;
static constexpr auto NUM_STRIPS          = HEIGHT_MAP_HEIGHT - 1;
static constexpr auto NUM_VERTS_PER_STRIP = HEIGHT_MAP_WIDTH * 2;

// camera
static auto camera           = Camera({0.0f, 3.0f, 10.0f}, {0.0f, 1.0f, 0.0f}, -90.f, -20.0f);
static const auto projection = glm::ortho(-2.f, 2.f, -1.5f, 1.5f, 0.1f, 100.0f);
// static const auto projection =
//     glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f,
//     100.0f);


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

    // draw in wireframe
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    std::vector<glm::vec3> vertices;
    for (auto i = 0; i < HEIGHT_MAP_HEIGHT; i++)
        for (auto j = 0; j < HEIGHT_MAP_WIDTH; j++)
            vertices.emplace_back(
                (0.5 + -HEIGHT_MAP_HEIGHT / 2) + j, 0.0f, (0.5 + -HEIGHT_MAP_HEIGHT / 2) + i);

    std::vector<uint32_t> indices;
    for (auto i = 0; i < HEIGHT_MAP_HEIGHT - 1; i++) {
        for (auto j = 0; j < HEIGHT_MAP_WIDTH; j++) {
            indices.push_back(i * HEIGHT_MAP_WIDTH + j);
            indices.push_back((i + 1) * HEIGHT_MAP_WIDTH + j);
        }
    }

    uint32_t VAO, VBO, EBO;

    glGenVertexArrays(1, &VAO);

    glGenBuffers(1, &VBO);

    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        std::size(vertices) * sizeof(glm::vec3),
        std::data(vertices),
        GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        std::size(indices) * sizeof(uint32_t),
        std::data(indices),
        GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    Shader shader("shaders/heightmap.vs", "shaders/heightmap.fs");
    shader.use();
    shader.set("projection", projection);
    shader.set("view", camera.GetViewMatrix());
    auto model = glm::mat4(1.0f);
    model      = glm::scale(model, {.25, .25, .25});
    shader.set("model", model);

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

        shader.use();
        glBindVertexArray(VAO);
        for (auto strip = 0; strip < NUM_STRIPS; strip++)
            glDrawElements(
                GL_TRIANGLE_STRIP,
                NUM_VERTS_PER_STRIP,
                GL_UNSIGNED_INT,
                (void*)(sizeof(uint32_t) * NUM_VERTS_PER_STRIP * strip));


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

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
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

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(yoffset);
}
