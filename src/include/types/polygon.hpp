#pragma once
#include <vector>
#include <tuple>
#include "rect.hpp"

class Polygon{
    protected:
        Rect boundingBox;
        int x, y;
        std::vector<std::pair<int, int>> points;
        bool resize;
    public:
        Polygon(int x, int y, int w, int h,bool resize=true) :
            boundingBox(x, y, w, h), points({{x, y}, {x+w, y}, {x+w, y+h}, {x, y+h}}), resize(resize) {
                if (resize) {
                    for (auto& point : points) {
                        point.first *= 2;
                        point.second *= 2;
                    }
                }
            }

        Polygon(const tmx::Object& object, bool resize = true)
            : boundingBox(object, resize), resize(resize) {
        
            x = static_cast<int>(object.getPosition().x);
            y = static_cast<int>(object.getPosition().y);
        
            for (const auto& p : object.getPoints()) {
                points.emplace_back(static_cast<int>(p.x), static_cast<int>(p.y));
            }
        }

        void move(int dx, int dy) {
            boundingBox.move(dx, dy);
            x += dx;
            y += dy;
        }

        bool on_screen() const {
            auto [left, right, top, bottom] = boundingBox.get_position();
            return right >= 0 && left < 960 && bottom > 0 && top < 544;
        }

        tmx::Object::Shape getShape() const{return tmx::Object::Shape::Polygon;};

        std::pair<int, int> collision(const Rect& other) const;
        std::pair<int, int> collision(const Rect& other, int dx, int dy) const;

        int get_position_x() const { return x; }
        int get_position_y() const { return y; }

        Rect get_bounding_box() const { return boundingBox; }

        std::vector<std::pair<int,int>> get_points() const {
            return points;
        };
};