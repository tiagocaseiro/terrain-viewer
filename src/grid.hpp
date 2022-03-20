#pragma once

#include <cstdint>

class Shader;
class Grid {
  public:
    Grid(int width, int height);
    void draw(Shader shader);

  private:
    int num_strips;
    int num_verts_per_strip;
    uint32_t VAO;
    uint32_t VBO;
    uint32_t EBO;
};