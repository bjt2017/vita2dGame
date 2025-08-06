#pragma once
#include "../objects.hpp"
#include "../assets.h"
#include "console.hpp"

#define COLLIDE_ZONE_Y 11
#define player_center_x (static_cast<int>(SCREEN_WIDTH / 2 - (PLAYER_REEL_WIDTH * ZOOM) / 2))
#define player_center_y (static_cast<int>(SCREEN_HEIGHT / 2 - (PLAYER_REEL_HEIGHT * ZOOM) / 2))

#define PLAYER_SPRITE_WIDTH 96  
#define PLAYER_SPRITE_HEIGHT 64 

#define PLAYER_REEL_POS_X 42
#define PLAYER_REEL_POS_Y 21

#define PLAYER_REEL_WIDTH 13
#define PLAYER_REEL_HEIGHT 18

struct Portal;
struct House;

enum Direction{
    LEFT,RIGHT,DOWN,UP
};

enum PlayerState{
    IDLE,
    WALK,
    AXE,
    SWIMMING,
};

struct PlayerStateData : public StateObject<PlayerState>{
    PlayerStateData(PlayerState state = PlayerState::IDLE, int nb_frame = 8, float frame_duration = 0.2f, bool blocking = false, int action_frame = -1)
        : StateObject<PlayerState>(state, nb_frame, frame_duration) {this->blocking = blocking; this->action_frame = action_frame;}
    vita2d_texture *texture_base;
    vita2d_texture *texture_hair;
    vita2d_texture *texture_tools;
    bool blocking = false;
    int action_frame = -1;
};

class Player : public Object<PlayerStateData, PlayerState> {
    protected :
        int speed;
        float time=0;
        bool can_interact = true;
        vita2d_pgf *font;
        std::pair<Direction, Direction> direction;
        House *interaction_house = nullptr;
        Portal *interaction_portal = nullptr;
        Position portal_entry_direction = Position::None;
    public :
        bool debug=false;
        Player(int x, int y);
        ~Player();
        void init_all_states() override;
        void draw() override;
        void update() override;
        void interaction() override;
        void change(PlayerState state);
        void move(int x, int y);

        Rect get_rect_collision() const override;

        int get_direction_x() const {return direction.first;}
        int get_direction_y() const {return direction.second;}

        int get_position_x() const {
            return rect.get_position_x();
        }
        int get_position_y() const {
            return rect.get_position_y();
        }

        std::tuple<int, int, int, int> get_position() const {
            return rect.get_position();
        }

        void set_direction_x(Direction dir) {direction.first = dir;}
        void set_direction_y(Direction dir) {direction.second = dir;}

        void set_can_interact(bool can) {
            can_interact = can;
        }
        bool get_can_interact() const {
            return can_interact;
        }
        Portal* get_interaction_portal() const {
            return interaction_portal;
        }
        void set_interaction_portal(Portal* portal){
            interaction_portal = portal;
        }
        Position get_portal_entry_direction() const {
            return portal_entry_direction;
        }
        void set_portal_entry_direction(Position direction) {
            portal_entry_direction = direction;
        }
        House* get_interaction_house() const {
            return interaction_house;
        }
        void set_interaction_house(House* house) {
            interaction_house = house;
        }
};