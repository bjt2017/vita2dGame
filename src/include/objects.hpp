#pragma once
#include <vita2d.h>
#include <unordered_set>
#include "types/polygon.hpp"

enum class ObjectType {
    PLAYER,
    TREE,
    COLLECTIBLE
};

template <typename U>
struct StateObject {
    U state;
    int nb_frame = 1;
    float frame_duration = 0.1f;
    StateObject(U state = {}, int nb_frame = 1, float frame_duration = 0.1f)
        : state(state), nb_frame(nb_frame), frame_duration(frame_duration) {}
};

template <class T, typename U>
class Object {
protected:
    Rect rect; 
    ObjectType type;         
    T current_state;   
    std::vector<T> all_states; //tab[ObjectType] = StateObject
    int frame=0;
    int layer;    
    float animation_timer=0.0f;      

public:
    Object(Rect rect, ObjectType type, int layer) : rect(rect), type(type), layer(layer) {}
    virtual ~Object() = default;

    virtual void draw() = 0;
    virtual void update() = 0;
    virtual void interaction() = 0;
    virtual void init_all_states() = 0;
    virtual Rect get_rect_collision() const = 0;

    // Object methods
    bool on_screen(){return rect.on_screen();};
    std::pair<int, int> collision(const Object<T,U>& other) const { return rect.collision(other.rect); }
    std::pair<int, int> collision(const Object<T,U>& other, int dx, int dy) const { return rect.collision(other.rect, dx, dy); }

    /*bool can_interact(const Player& player) const {
        return this->rect.collision(player.get_rect());
    }
    void move(int x, int y) = 0;*/

    // Getters
    int get_position_x() const { return rect.get_position_x(); }
    int get_position_y() const { return rect.get_position_y(); }
    int get_width() const { return rect.get_width(); }
    int get_height() const { return rect.get_height(); }
    int get_frame() const { return frame; }
    int get_layer() const { return layer; }

    Rect get_rect() const { return rect; }

    T get_current_state() const { return current_state; }

    T get_state(U type) const {
        for (const auto& state : all_states) {
            if (state.state == type) {
                return state;
            }
        }
        return all_states.empty() ? T() : all_states[0];
    }

    // Setters
    void set_position(int x, int y) { rect.move(x, y); }
};
