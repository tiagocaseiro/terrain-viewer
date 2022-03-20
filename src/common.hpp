#pragma once

#include <iostream>

#include <glm/glm.hpp>

template <typename... Types>
auto& operator<<(std::ostream& os, const glm::vec<1, Types...>& vec) {
    os << vec.x;
    return os;
}

template <typename... Types>
auto& operator<<(std::ostream& os, const glm::vec<2, Types...>& vec) {
    os << vec.x << " " << vec.y;
    return os;
}

template <typename... Types>
auto& operator<<(std::ostream& os, const glm::vec<3, Types...>& vec) {
    os << vec.x << " " << vec.y << " " << vec.z;
    return os;
}


template <typename... Types>
auto& operator<<(std::ostream& os, const glm::vec<4, Types...>& vec) {
    os << vec.x << " " << vec.y << " " << vec.z << " " << vec.w;
    return os;
}
