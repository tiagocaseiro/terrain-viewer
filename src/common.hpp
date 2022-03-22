#pragma once

#include <iostream>

#include <glm/glm.hpp>

inline auto& operator<<(std::ostream& os, const glm::vec1& vec) {
    os << vec.x;
    return os;
}

inline auto& operator<<(std::ostream& os, const glm::vec2& vec) {
    os << vec.x << " " << vec.y;
    return os;
}

inline auto& operator<<(std::ostream& os, const glm::vec3& vec) {
    os << vec.x << " " << vec.y << " " << vec.z;
    return os;
}

inline auto& operator<<(std::ostream& os, const glm::vec4& vec) {
    os << vec.x << " " << vec.y << " " << vec.z << " " << vec.w;
    return os;
}
