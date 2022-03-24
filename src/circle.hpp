#pragma once

#include <cstdint>

class Shader;

class Circle {
  public:
    Circle(uint32_t slices, float radius);
    void draw() const;

  private:
    uint32_t count;
    uint32_t VAO;
    uint32_t VBO;
    uint32_t EBO;
};