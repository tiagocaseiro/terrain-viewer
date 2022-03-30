#pragma once

#include <array>

#include <glm/glm.hpp>

#include <glad/glad.h>

template <size_t width, size_t height>
class Grid {
  public:
    Grid(
        std::array<glm::vec3, width* height> vertices         = GenerateVertices(),
        std::array<uint32_t, (height - 1) * width* 2> indices = GenerateIndices()) {

        glGenVertexArrays(1, &VAO_);

        glGenBuffers(1, &VBO_);

        glGenBuffers(1, &EBO_);

        glBindVertexArray(VAO_);

        glBindBuffer(GL_ARRAY_BUFFER, VBO_);
        glBufferData(
            GL_ARRAY_BUFFER,
            std::size(vertices) * sizeof(glm::vec3),
            std::data(vertices),
            GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_);
        glBufferData(
            GL_ELEMENT_ARRAY_BUFFER,
            std::size(indices) * sizeof(uint32_t),
            std::data(indices),
            GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
    }

    void draw() {
        constexpr auto num_strips{height - 1};
        constexpr auto num_verts_per_strip{width * 2};
        glBindVertexArray(VAO_);
        for (auto strip = 0; strip < num_strips; strip++)
            glDrawElements(
                GL_TRIANGLE_STRIP,
                num_verts_per_strip,
                GL_UNSIGNED_INT,
                (void*)(sizeof(uint32_t) * num_verts_per_strip * strip));
    }

    static constexpr auto GenerateVertices() {
        std::array<glm::vec3, width * height> vertices;
        for (auto i = 0; i < height; i++)
            for (auto j = 0; j < width; j++)
                vertices[i * width + j] = {
                    0.5f - (width / 2.f) + j, 0.0f, 0.5f - (height / 2.f) + i};
        return vertices;
    }


    static constexpr auto GenerateIndices() {
        constexpr auto num_strips{height - 1};
        constexpr auto num_verts_per_strip{width * 2};
        std::array<uint32_t, num_strips * num_verts_per_strip> indices;
        for (auto i = 0, c = 0; i < height - 1; i++) {
            for (auto j = 0; j < width; j++) {
                indices[c++] = (i * width + j);
                indices[c++] = ((i + 1) * width + j);
            }
        }
        return indices;
    }

  private:
    uint32_t VAO_;
    uint32_t VBO_;
    uint32_t EBO_;
};