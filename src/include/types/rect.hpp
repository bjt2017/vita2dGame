#pragma once

#include "../console.hpp"
#include <tuple>
#include <tmxlite/Object.hpp>

class Polygon;

enum class Position{ None, Top, Bottom, Left, Right };


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

    bool intersects(const Rect& other) const {
        auto [left, right, top, bottom] = get_position();
        auto [other_left, other_right, other_top, other_bottom] = other.get_position();
        return !(right < other_left || left > other_right || bottom < other_top || top > other_bottom);
    }

    std::pair<Position,Position> get_position_relative(const Rect& other) const {
        auto [l, r, t, b]       = this->get_position();
        auto [ol, or_, ot, ob] = other.get_position();

        // Calcule le centre vertical et horizontal
        int centerY = (t + b) / 2;
        int centerX = (l + r) / 2;
        int otherCenterY = (ot + ob) / 2;
        int otherCenterX = (ol + or_) / 2;

        // Vertical : si intersect → distance aux bords comme avant
        Position pos_y;
        if (this->intersects(other)) {
            int dTop    = (b >= ot) ? (b - ot) : (ot - b);
            int dBottom = (t >= ob) ? (t - ob) : (ob - t);
            pos_y = (dTop <= dBottom) ? Position::Top : Position::Bottom;
        } else {
            // en-dehors → comparer les centres
            pos_y = (centerY < otherCenterY) ? Position::Top : Position::Bottom;
        }

        // Horizontal : même logique
        Position pos_x;
        if (this->intersects(other)) {
            int dLeft  = (l >= ol)  ? (l - ol)  : (ol  - l);
            int dRight = (r >= or_) ? (r - or_) : (or_ - r);
            pos_x = (dLeft <= dRight) ? Position::Left : Position::Right;
        } else {
            pos_x = (centerX < otherCenterX) ? Position::Left : Position::Right;
        }

        return { pos_y, pos_x };
    }




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