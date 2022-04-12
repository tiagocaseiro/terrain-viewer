#pragma once

#include <filesystem>
#include <thread>

#include "grid.hpp"

static auto GenerateHeightMap(uint32_t width, uint32_t height, const glm::mat4& model) {
    auto vertices = Grid::GenerateVertices(width, height);
    for (auto& v : vertices) {
        auto n = glm::vec4(v.x, v.y, v.z, 1.0f) * model;
        v.x    = n.x;
        v.y    = n.y;
        v.z    = n.z;
    }
    return vertices;
}

template <typename Shape>
class Editor {
  public:
    Editor(uint32_t width, uint32_t height, Cursor<Shape> cursor, glm::mat4 model = glm::mat4(1.0f))
      : vertices_{GenerateHeightMap(width, height, model)}, width_{width}, height_{height},
        grid_{width, height, vertices_, Grid::GenerateIndices(width, height)}, cursor_{cursor} {}

    auto onMouseMovement(float xoffset, float zoffset) { cursor_.updatePosition(xoffset, zoffset); }

    auto onMouseButton(int button, int action) {
        // if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        //     vertices_[0].y += increment_;
        //     grid_ = {width_, height_, vertices_, Grid::GenerateIndices(width_, height_)};
        // }
    }

    auto draw(Shader triangle_shader, Shader wireframe_shader, Shader cursor_shader) {
        triangle_shader.use();
        triangle_shader.set("color", glm::vec3{1.0f});
        triangle_shader.set("model", glm::mat4(1.0f));
        grid_.draw();

        glDepthFunc(GL_ALWAYS);
        wireframe_shader.use();
        wireframe_shader.set("color", glm::vec3{0.0f});
        wireframe_shader.set("model", glm::mat4(1.0f));
        grid_.draw();
        glDepthFunc(GL_LESS);

        glDepthFunc(GL_ALWAYS);
        cursor_shader.use();
        auto cursor_model = glm::translate(glm::mat4(1.0f), cursor_.getPosition());
        cursor_model      = glm::rotate(cursor_model, glm::radians(90.f), {1.0f, 0.0f, 0.0f});
        cursor_shader.set("model", cursor_model);
        cursor_shader.set("color", cursor_.getColor());
        cursor_.draw();
        glDepthFunc(GL_LESS);
    }

    void save(std::filesystem::path path) {
        std::thread{[path = std::move(path)] {
            std::cout << "Sucessfully exported to " << path.string() << std::endl;
        }}.detach();
    }

  private:
    std::vector<glm::vec3> vertices_;
    uint32_t width_;
    uint32_t height_;
    Grid grid_;
    Cursor<Shape> cursor_;
    glm::mat4 model_;
    float increment_ = 0.05f;
    float max_       = 10.f;
    float min_       = -10.f;
};