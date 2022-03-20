#include "grid.hpp"

#include <vector>

#include <glad/glad.h>

#include <glm/glm.hpp>

#include <learnopengl/shader.hpp>

#include "common.hpp"

Grid::Grid(int width, int height) : num_strips{height - 1}, num_verts_per_strip{width * 2} {
    std::vector<glm::vec3> vertices;
    for (auto i = 0; i < height; i++)
        for (auto j = 0; j < width; j++)
            vertices.emplace_back((0.5f - (width / 2.f)) + j, 0.0f, (0.5f - (height / 2.f)) + i);

    std::vector<uint32_t> indices;
    for (auto i = 0; i < height - 1; i++) {
        for (auto j = 0; j < width; j++) {
            indices.push_back(i * width + j);
            indices.push_back((i + 1) * width + j);
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
}

void Grid::draw(Shader shader) {
    shader.use();
    glBindVertexArray(VAO);
    for (auto strip = 0; strip < num_strips; strip++)
        glDrawElements(
            GL_TRIANGLE_STRIP,
            num_verts_per_strip,
            GL_UNSIGNED_INT,
            (void*)(sizeof(uint32_t) * num_verts_per_strip * strip));
}