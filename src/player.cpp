#include "include/player.hpp"

Player::Player(int x, int y, int speed_value) {
    posx = x;
    posy = y;
    speed = speed_value;
    frame = 0;
    frame_duration = 0.1f;  
    animation_timer = 0.0f;
    direction = RIGHT; // Par défaut, le joueur est tourné vers la droite
    state = IDLE; // Par défaut, l'état est idle
    texture_walk = vita2d_load_PNG_buffer(&_binary_assets_base_walk_strip8_png_start);
    texture_idle = vita2d_load_PNG_buffer(&_binary_assets_base_idle_strip9_png_start);
    texture_bowlhair_idle = vita2d_load_PNG_buffer(&_binary_assets_bowlhair_idle_strip9_png_start);
    font = vita2d_load_default_pgf();
    debug = false;
}

Player::~Player() {
    vita2d_free_texture(texture_walk);
    vita2d_free_texture(texture_idle);
    vita2d_free_texture(texture_bowlhair_idle);
    vita2d_free_texture(tileset);
    vita2d_free_pgf(font);
}

void Player::move(int x, int y) {
    posx += x * speed;
    posy += y * speed;

    if (x < 0) {
        direction = LEFT;
    } else if (x > 0) {
        direction = RIGHT;
    }

    state = (x != 0 || y != 0) ? WALK : IDLE;
}

void Player::update_animation(float delta_time) {
    animation_timer += delta_time;
    if (animation_timer >= frame_duration) {
        frame = (frame + 1) % 8;
        animation_timer = 0.0f;
    }
}

void Player::draw() {
    int src_x = PLAYER_SPRITE_X + frame * (PLAYER_SPRITE_SPACING + PLAYER_SPRITE_WIDTH);
    float flip = (direction == LEFT) ? -ZOOM : ZOOM;
    float draw_x = (direction == LEFT) ? posx + (PLAYER_SPRITE_WIDTH * ZOOM) + 1 : posx;

    if (state == WALK) {
        vita2d_draw_texture_part_scale(texture_walk, draw_x, posy, 
                                       src_x, PLAYER_SPRITE_Y, PLAYER_SPRITE_WIDTH, PLAYER_SPRITE_HEIGHT, 
                                       flip, ZOOM);
    } else {
        vita2d_draw_texture_part_scale(texture_idle, draw_x, posy, 
                                       src_x, PLAYER_SPRITE_Y, PLAYER_SPRITE_WIDTH, PLAYER_SPRITE_HEIGHT, 
                                       flip, ZOOM);
    }

    vita2d_draw_texture_part_scale(texture_bowlhair_idle, draw_x, posy, 
                                   src_x, PLAYER_SPRITE_Y, PLAYER_SPRITE_WIDTH, PLAYER_SPRITE_HEIGHT, 
                                   flip, ZOOM);

    if (debug) {
        vita2d_draw_line(posx, posy, posx + PLAYER_SPRITE_WIDTH * ZOOM, posy, RGBA8(255, 0, 0, 255));
        vita2d_draw_line(posx, posy, posx, posy + PLAYER_SPRITE_HEIGHT * ZOOM, RGBA8(255, 0, 0, 255)); 
        vita2d_draw_line(posx + PLAYER_SPRITE_WIDTH * ZOOM, posy, posx + PLAYER_SPRITE_WIDTH * ZOOM, posy + PLAYER_SPRITE_HEIGHT * ZOOM, RGBA8(255, 0, 0, 255)); 
        vita2d_draw_line(posx, posy + PLAYER_SPRITE_HEIGHT * ZOOM, posx + PLAYER_SPRITE_WIDTH * ZOOM, posy + PLAYER_SPRITE_HEIGHT * ZOOM, RGBA8(255, 0, 0, 255)); 
        vita2d_draw_rectangle(posx, posy + (PLAYER_SPRITE_HEIGHT-(PLAYER_SPRITE_HEIGHT - COLLIDE_ZONE_Y))*ZOOM, // Position
         PLAYER_SPRITE_WIDTH * ZOOM, (PLAYER_SPRITE_HEIGHT - COLLIDE_ZONE_Y)*ZOOM, // Taille
          RGBA8(255, 0, 0, 255));
    }

}
