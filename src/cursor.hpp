#pragma once

template <typename Shape>
class Cursor {
  public:
    explicit Cursor(Shape shape, float speed = 0.05f, glm::vec3 color = {1.0f, 1.0f, 1.0f})
      : speed_{speed}, color_{color}, position_{}, shape_{std::move(shape)} {}

    void updatePosition(float xoffset, float zoffset) {
        position_.x += (xoffset * speed_);
        position_.z += (-zoffset * speed_);
    }

    auto getPosition() const { return position_; }

    auto getColor() const { return color_; }

    void draw(Shader shader) const { shape_.draw(shader); }

  private:
    float speed_;
    glm::vec3 color_;
    glm::vec3 position_;
    Shape shape_;
};