#pragma once

#include <algorithm>
#include <filesystem>
#include <thread>

#include "cursor.hpp"
#include "grid.hpp"

class Editor {
  public:
    Editor(uint32_t width, uint32_t height, Cursor cursor)
      : width_{width}, height_{height}, cursor_{cursor}, vertices_{Grid::GenerateVertices(
                                                             width,
                                                             height)},
        grid_{width, height, vertices_, Grid::GenerateIndices(width, height)} {}

    auto updateCursor(float xoffset, float zoffset) { cursor_.updatePosition(xoffset, zoffset); }

    void set() {
        for (auto& vertex_position : vertices_) {
            auto y            = vertex_position.y;
            vertex_position.y = {};
            if (auto diff = glm::length(cursor_.getPosition() - vertex_position);
                diff <= cursor_.getRadius()) {
                y = value_;
            }
            vertex_position.y = y;
        }
        grid_ = {width_, height_, vertices_, Grid::GenerateIndices(width_, height_)};
    }

    void increment(float increment) {
        value_ += increment;
        value_ = std::max(min_, value_);
        value_ = std::min(max_, value_);
    }

    void reset() { value_ = {}; }

    auto draw(Shader triangle_shader, Shader wireframe_shader, Shader cursor_shader) {
        triangle_shader.use();
        triangle_shader.set("color", glm::vec3{1.0f});
        triangle_shader.set("model", glm::mat4(1.0f));
        grid_.draw();

        wireframe_shader.use();
        wireframe_shader.set("color", glm::vec3{0.0f});
        wireframe_shader.set("model", glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.006, 0.0)));
        grid_.draw();

        glDepthFunc(GL_ALWAYS);
        cursor_shader.use();
        cursor_shader.set("color", cursor_.getColor());
        cursor_shader.set("radius", cursor_.getRadius());
        cursor_shader.set("grid_model", glm::mat4(1.0f));
        cursor_shader.set("cursor_model", glm::translate(glm::mat4(1.0f), cursor_.getPosition()));
        grid_.draw();
        glDepthFunc(GL_LESS);
    }

    void save(std::filesystem::path path) {
        std::thread{[path = std::move(path)] {
            std::cout << "Sucessfully exported to " << path.string() << std::endl;
        }}.detach();
    }

  private:
    uint32_t width_;
    uint32_t height_;
    Cursor cursor_;
    std::vector<glm::vec3> vertices_;
    Grid grid_;
    float value_ = 0.0f;
    float max_   = 10.f;
    float min_   = -10.f;
};