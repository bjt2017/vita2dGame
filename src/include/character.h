#pragma once
#include "objects/interactableContainer.h"
#include "objects/player.hpp"

#define COLLIDE_ZONE_Y 11
#define player_center_x (static_cast<int>(SCREEN_WIDTH / 2 - (PLAYER_REEL_WIDTH * ZOOM) / 2))
#define player_center_y (static_cast<int>(SCREEN_HEIGHT / 2 - (PLAYER_REEL_HEIGHT * ZOOM) / 2))

#define PLAYER_SPRITE_WIDTH 96  
#define PLAYER_SPRITE_HEIGHT 64 

#define PLAYER_REEL_POS_X 42
#define PLAYER_REEL_POS_Y 21

#define PLAYER_REEL_WIDTH 13
#define PLAYER_REEL_HEIGHT 18

class Character : public InteractableContainer {
public:
    Character(const std::pair<int, int> position, const std::string& name, const std::string& description)
        : InteractableContainer({position.first-16, position.second-16, PLAYER_REEL_WIDTH+32, PLAYER_REEL_HEIGHT+32}, InteractableType::Dialogue, name, description), rect({position.first, position.second, PLAYER_REEL_WIDTH, PLAYER_REEL_HEIGHT}), direction(Direction::RIGHT) {}
    ~Character() = default;

    void interact() {
        Console::error("Character interaction not implemented.");
    }
    void update(Player &player, const SceCtrlData& pad, const SceCtrlData& previousPad, bool is_in_dialogue);
    static void load_assets();
    static void free_texture();
    void draw() const;
    Rect get_rect() const {
        return rect;
    }
    void set_rect(const Rect& newRect) {
        rect = newRect;
    }
    void set_direction(Direction dir) {
        direction = dir;
    }
    Direction get_direction() const {
        return direction;
    }

private:
    Rect rect;
    float timer=0;
    float frame_duration = 0.2f;
    int frame=0;
    Direction direction;
    static vita2d_texture *texture_idle_base;
    static vita2d_texture *texture_idle_hair;
    static vita2d_texture *texture_idle_tools;
    static vita2d_texture *texture_dialogue_interaction;
    static vita2d_texture *texture_shadow;

};
