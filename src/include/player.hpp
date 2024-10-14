#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "main.hpp"

class Player {
public:
    int posx;
    int posy;
    int speed;
    int frame;
    float frame_duration;
    float animation_timer;
    int direction;
    int state;

    bool debug;


    vita2d_texture *texture_idle;
    vita2d_texture *texture_walk;
    vita2d_texture *texture_bowlhair_idle;
    vita2d_pgf *font;

    Player(int x, int y, int speed_value);

    ~Player();

    void move(int x, int y);

    void update_animation(float delta_time);

    void draw();


};


#endif
