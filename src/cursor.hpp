#pragma once


template <typename Shape>
class Cursor {
  public:
    Cursor(const Shape& shape) : shape_{shape} {}
    void draw(Shader shader) { shape_.draw(shader); }

  private:
    Shape shape_;
};