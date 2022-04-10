#pragma once

#include <vector>

#include <glm/glm.hpp>

#include <glad/glad.h>

class Grid {
  public:
    Grid(uint32_t width, uint32_t height);

    Grid(
        uint32_t width,
        uint32_t height,
        std::vector<glm::vec3> vertices,
        std::vector<uint32_t> indices);

    void draw();


    static auto GenerateVertices(uint32_t width, uint32_t height) {
        std::vector<glm::vec3> vertices;
        for (auto i = 0; i < height; i++)
            for (auto j = 0; j < width; j++)
                vertices.emplace_back(0.5f - (width / 2.f) + j, 0.0f, 0.5f - (height / 2.f) + i);
        return vertices;
    }

    static auto GenerateIndices(uint32_t width, uint32_t height) {
        std::vector<uint32_t> indices;
        for (auto i = 0, c = 0; i < height - 1; i++) {
            for (auto j = 0; j < width; j++) {
                indices.push_back(i * width + j);
                indices.push_back((i + 1) * width + j);
            }
        }
        return indices;
    }

  private:
    uint32_t VAO_;
    uint32_t VBO_;
    uint32_t EBO_;
    uint32_t width_;
    uint32_t height_;
};