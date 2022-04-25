#pragma once

class Cursor {
  public:
    explicit Cursor(float speed, glm::vec3 color, float radius)
      : speed_{speed}, color_{std::move(color)}, position_{}, radius_{radius} {}

    void updatePosition(float xoffset, float zoffset) {
        position_.x += (xoffset * speed_);
        position_.z += (-zoffset * speed_);
    }

    auto getPosition() const { return position_; }

    auto getColor() const { return color_; }

    auto getRadius() const { return radius_; }

  private:
    float speed_;
    glm::vec3 color_;
    glm::vec3 position_;
    float radius_;
};