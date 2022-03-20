#include "circle.hpp"

#include <numeric>
#include <vector>

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/shader.hpp>

Circle::Circle(uint32_t slices, float radius) {
    std::vector<glm::vec3> vertices;
    vertices.push_back({});

    const auto step = 360.f / slices;

    for (auto i = 0; i < slices; i++)
        vertices.emplace_back(
            glm::cos(glm::radians(step * i)) * radius,
            glm::sin(glm::radians(step * i)) * radius,
            0.0f);

    std::vector<uint32_t> indices(std::size(vertices));
    std::iota(indices.begin(), indices.end(), 0);
    indices.push_back(1);

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

    count = std::size(indices);
}

void Circle::draw(Shader shader) {
    shader.use();
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLE_FAN, count, GL_UNSIGNED_INT, nullptr);
}