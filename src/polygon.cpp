#include "include/types/polygon.hpp"

std::pair<int, int> Polygon::collision(const Rect& other) const {
    return boundingBox.collision(other);
}

std::pair<int, int> Polygon::collision(const Rect& other, int dx, int dy) const {
    return boundingBox.collision(other, dx, dy);
}