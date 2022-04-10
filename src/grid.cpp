#include "grid.hpp"

Grid::Grid(uint32_t width, uint32_t height)
  : Grid(width, height, GenerateVertices(width, height), GenerateIndices(width, height)) {}

Grid::Grid(
    uint32_t width,
    uint32_t height,
    std::vector<glm::vec3> vertices,
    std::vector<uint32_t> indices)
  : width_{width}, height_{height} {

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

void Grid::draw() {
    auto num_strips{height_ - 1};
    auto num_verts_per_strip{width_ * 2};
    glBindVertexArray(VAO_);
    for (auto strip = 0; strip < num_strips; strip++)
        glDrawElements(
            GL_TRIANGLE_STRIP,
            num_verts_per_strip,
            GL_UNSIGNED_INT,
            (void*)(sizeof(uint32_t) * num_verts_per_strip * strip));
}
