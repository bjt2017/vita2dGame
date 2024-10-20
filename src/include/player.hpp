#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "globals.hpp"
#include "map.hpp"

#define player_center_x (SCREEN_WIDTH / 2 - (PLAYER_SPRITE_WIDTH*ZOOM) / 2) + (PLAYER_REEL_POS_X*ZOOM)
#define player_center_y (SCREEN_HEIGHT / 2 - (PLAYER_SPRITE_HEIGHT*ZOOM) / 2) + (PLAYER_REEL_POS_Y*ZOOM)

class Map;

struct State{
    vita2d_texture *texture_base;
    vita2d_texture *texture_hair;
    vita2d_texture *texture_tools;
    bool blocking = false;
    bool can_move = true;
    int nb_frame = 8;
};

class Player {
public:
    int posx;
    int posy;

    int posx_player;
    int posy_player;

    int speed;
    int frame;
    float frame_duration;
    float animation_timer;
    int direction_x;
    int direction_y;
    int state;
    
    int current_layer;

    bool debug;

    State current_state;
    State idle;
    State walk;
    State axe;

    vita2d_pgf *font;

    Player(int x, int y, int speed_value);

    ~Player();
    std::tuple<int, int,int,int> getBoundingBox(const tmx::Object& object, float zoom, int mapPosX, int mapPosY);
    std::pair<int, int> collision_rect(tmx::Object object, Map *map, int x, int y);
    std::pair<int, int> collision_polygon(tmx::Object object, Map *map, int x, int y);

    std::pair<int, int> collision_bounding_box(std::tuple<int, int,int,int> boundingBox, Map *map, int x, int y);

    bool collision_bounding_box(std::tuple<int, int, int, int> boundingBox, Map* map);



    void changeState(int new_state);

    void move(int x, int y);

    void update_animation(float delta_time);

    void draw();


};


#endif
