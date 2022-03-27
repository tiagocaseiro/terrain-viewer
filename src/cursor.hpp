#pragma once

template <typename Shape>
class Cursor {
  public:
    explicit Cursor(Shape shape, float speed, glm::vec3 color)
      : speed_{speed}, color_{std::move(color)}, position_{}, shape_{std::move(shape)} {}

    void updatePosition(float xoffset, float zoffset) {
        position_.x += (xoffset * speed_);
        position_.z += (-zoffset * speed_);
    }

    void setOnLeftClick(std::function<void(void)> callable) { onLeftClickCallback_ = callable; }

    void onLeftClick() const {
        if (onLeftClickCallback_)
            onLeftClickCallback_();
    }

    auto getPosition() const { return position_; }

    auto getColor() const { return color_; }

    void draw() const { shape_.draw(); }

  private:
    float speed_;
    glm::vec3 color_;
    glm::vec3 position_;
    Shape shape_;
    std::function<void(void)> onLeftClickCallback_;
};