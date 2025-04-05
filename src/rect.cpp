#include "include/types/rect.hpp"

int Rect::map_pos_x = 0;
int Rect::map_pos_y = 0;
float Rect::zoom = 3.0f;

Rect::Rect(const tmx::Object& object, bool resize) : resize(resize) {
    if (object.getShape() == tmx::Object::Shape::Rectangle) {
        x = object.getAABB().left;
        y = object.getAABB().top;
        width = object.getAABB().width;
        height = object.getAABB().height;
    } else if (object.getShape() == tmx::Object::Shape::Polygon || object.getShape() == tmx::Object::Shape::Polyline) {
        const auto& points = object.getPoints();
        if (points.size() < 2) {
            x = 0;
            y = 0;
            width = 0;
            height = 0;
        }
        const auto& position = object.getPosition();
        int minX = points[0].x, maxX = points[0].x;
        int minY = points[0].y, maxY = points[0].y;
        for (const auto& point : points) {
            if (point.x < minX) minX = point.x;
            if (point.x > maxX) maxX = point.x;
            if (point.y < minY) minY = point.y;
            if (point.y > maxY) maxY = point.y;
        }
        x = (position.x + minX);
        y = (position.y + minY);
        width = (maxX - minX);
        height = (maxY - minY);
    } else {
        x = 0;
        y = 0;
        width = 0;
        height = 0;
    }
}

std::pair<int, int> Rect::collision(const Rect& other, int dx, int dy) const
{
        int maxX = dx;
        int maxY = dy;

        auto [this_left, this_right, this_top, this_bottom] = get_position();
        auto [other_left, other_right, other_top, other_bottom] = other.get_position();
     
        if (this_bottom > other_top && this_top < other_bottom)
        {
            if (dx > 0 && this_right + dx > other_left && this_left + dx < other_right)
            {
                maxX = std::min(maxX, other_left - this_right);
            }
            else if (dx < 0 && this_left + dx < other_right && this_right + dx > other_left)
            {
                maxX = std::max(maxX, other_right - this_left);
            }
        }

        if ((this_right + maxX) > other_left && (this_left + maxX) < other_right)
        {
            if (dy > 0 && this_bottom + dy > other_top && this_top + dy < other_bottom)
            {
                maxY = std::min(maxY, other_top - this_bottom);
            }
            else if (dy < 0 && (this_top + dy) < other_bottom && (this_bottom + dy) > other_top)
            {
                maxY = std::max(maxY, other_bottom - this_top);
            }
        }

        return { maxX, maxY };
}

std::pair<int, int> Rect::collision(const Rect& other) const
{
    auto [this_left, this_right, this_top, this_bottom] = get_position();
    auto [other_left, other_right, other_top, other_bottom] = other.get_position();
     
    if (this_right  <= other_left  || this_left   >= other_right  ||
        this_bottom <= other_top   || this_top    >= other_bottom)
    {
        return {0, 0};
    }

    int overlapX1 = this_right - other_left;   
    int overlapX2 = other_right - this_left; 
    int overlapX  = (overlapX1 < overlapX2) ? overlapX1 : -overlapX2;

    int overlapY1 = this_bottom - other_top;   
    int overlapY2 = other_bottom - this_top;
    int overlapY  = (overlapY1 < overlapY2) ? overlapY1 : -overlapY2;

    return {overlapX, overlapY};
}