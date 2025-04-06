#pragma once

#include "../console.hpp"
#include <tuple>
#include <tmxlite/Object.hpp>

class Polygon;

class Rect {
protected:
    static int map_pos_x,map_pos_y;
    static float zoom;
    int x, y, width, height;
    bool resize;

public:
    Rect(int x, int y, int w, int h,bool resize=true) : x(x), y(y), width(w), height(h), resize(resize) {}
    Rect(const tmx::Object& object, bool resize=true);
    
    void move(int dx, int dy) {
        x += dx;
        y += dy;
    }

    std::pair<int, int> collision(const Rect& other) const;
    std::pair<int, int> collision(const Rect& other, int dx, int dy) const;

    std::pair<int, int> collision(const Polygon& other) const;
    std::pair<int, int> collision(const Polygon& other, int dx, int dy) const;

    bool on_screen() const {
        auto [left, right, top, bottom] = get_position();
        return right >= 0 && left < 960 && bottom > 0 && top < 544;
    }

    //revoir
    tmx::Object::Shape getShape() const{return tmx::Object::Shape::Rectangle;};
    // Getters
    int get_position_x() const { return x; }
    int get_position_y() const { return y; }
    int get_width() const { return width; }
    int get_height() const { return height; }

    std::tuple<int, int, int, int> get_position() const {
        int this_left   = resize ? x*zoom - map_pos_x : x;
        int this_right  = resize ? x*zoom + width*zoom - map_pos_x : x + width;
        int this_top    = resize ? y*zoom - map_pos_y : y;
        int this_bottom = resize ? y*zoom + height*zoom - map_pos_y : y + height;
        return std::make_tuple(this_left, this_right, this_top, this_bottom);
    }

    // Setters
    static void set_map_pos(int x, int y, float z) {
        map_pos_x = x;
        map_pos_y = y;
        zoom = z;
    }

    static int get_map_pos_x() {
        return map_pos_x;
    }
    static int get_map_pos_y() {
        return map_pos_y;
    }
    
    static const float& get_zoom() {
        return zoom;
    }
};