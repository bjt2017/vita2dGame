#include "include/character.h"
#include <vita2d.h>

vita2d_texture *Character::texture_idle_base = nullptr;
vita2d_texture *Character::texture_idle_hair = nullptr;
vita2d_texture *Character::texture_idle_tools = nullptr;
vita2d_texture *Character::texture_dialogue_interaction = nullptr;
vita2d_texture *Character::texture_shadow = nullptr;

DialogueManager dialogueManager = DialogueManager();

void Character::load_assets() {
    texture_idle_base = vita2d_load_PNG_buffer(&_binary_assets_Characters_Idle_base_png_start);
    texture_idle_hair = vita2d_load_PNG_buffer(&_binary_assets_Characters_Idle_longhair_png_start);
    texture_idle_tools = vita2d_load_PNG_buffer(&_binary_assets_Characters_Idle_tools_png_start);
    texture_dialogue_interaction = vita2d_load_PNG_buffer(&_binary_assets_UI_expression_chat_png_start);
    texture_shadow = vita2d_load_PNG_buffer(&_binary_assets_Characters_shadow_png_start);
}

void Character::free_texture() {
    if (texture_idle_base) vita2d_free_texture(texture_idle_base);
    if (texture_idle_hair) vita2d_free_texture(texture_idle_hair);
    if (texture_idle_tools) vita2d_free_texture(texture_idle_tools);
    if (texture_dialogue_interaction) vita2d_free_texture(texture_dialogue_interaction);
    if (texture_shadow) vita2d_free_texture(texture_shadow);
}

void Character::update(Player &player, const SceCtrlData& pad, const SceCtrlData& previousPad, bool is_in_dialogue) {
    if (!rect.on_screen()) return;
    if (!is_in_dialogue) {
        if (getBoundingBox().intersects(player.get_rect_collision())) {
            if (pad.buttons & SCE_CTRL_TRIANGLE && !(previousPad.buttons & SCE_CTRL_TRIANGLE)) {
                // Position du joueur et du personnage
                const auto [char_left, char_right, char_top, char_bottom] = rect.get_position();
                const auto [player_left, player_right, player_top, player_bottom] = player.get_position();

                // Calculer la direction horizontale
                int move_x = 0;
                int move_y = 0;

                int direction = 0;
                if (player_left < char_left) {
                    // Le joueur est à gauche du NPC → il doit aller à droite
                    move_x = (char_left - player_right) - 10;
                    direction = 1;
                } else if (player_right >= char_right) {
                    // Le joueur est à droite du NPC → il doit aller à gauche
                    move_x = (char_right - player_left) + 11;
                    direction = -1;
                }

                // Aligner verticalement juste devant
                move_y = (char_bottom - player_bottom);

                this->set_direction(direction != 1 ? Direction::RIGHT : Direction::LEFT);
                player.play_cutscene({
                {
                    CutsceneStepType::Move,
                    {move_x, move_y},
                    0.5f
                },
                {
                    CutsceneStepType::Move,
                    {direction, 0},
                },
                {
                    CutsceneStepType::Callback,
                    {0,0},
                    0.0f,
                    0.0f,
                    0.0f, 
                    "",   
                    []() {
                        dialogueManager.start({
                            {"NPC", "Bonjour, comment ça va ?"},
                            {"Player", "Ça va bien, merci ! Et toi ?"},
                            {"NPC", "Je vais bien aussi, merci de demander !"},
                        });
                    }
                }
            });
            }
        }
    }

    timer += 1.0f / 60.0f; 
    if (timer > frame_duration) {
        timer = 0;
        frame++;
        if (frame >= 8) {
            frame = 0;
        }
    }
}

void Character::draw() const{
    if (!rect.on_screen()) return;
    int src_x = PLAYER_SPRITE_WIDTH * frame;
    float zoom = Rect::get_zoom();
    float posx = rect.get_position_x() * zoom - Rect::get_map_pos_x();
    float posy = rect.get_position_y() * zoom - Rect::get_map_pos_y();

    float flip = (get_direction() == Direction::LEFT) ? -zoom : zoom;

    float draw_x = (get_direction() == Direction::LEFT) ? posx - PLAYER_REEL_POS_X*zoom + (PLAYER_SPRITE_WIDTH * zoom) + 3 : posx - PLAYER_REEL_POS_X*zoom;
    float draw_y = posy - PLAYER_REEL_POS_Y*zoom;

    vita2d_draw_texture_scale(texture_shadow, 
        posx - 3*zoom + (flip > 0 ? 0 : zoom), posy + PLAYER_REEL_HEIGHT * zoom - 4 * zoom,
        zoom, zoom
    );
    
    vita2d_draw_texture_part_scale(texture_idle_base, 
        draw_x, draw_y,
        src_x, 0, PLAYER_SPRITE_WIDTH, PLAYER_SPRITE_HEIGHT,
        flip, zoom
    );

    vita2d_draw_texture_part_scale(texture_idle_hair, 
        draw_x,
        draw_y,
        src_x, 0, PLAYER_SPRITE_WIDTH, PLAYER_SPRITE_HEIGHT,
        flip, zoom
    );

    vita2d_draw_texture_part_scale(texture_idle_tools, 
        draw_x,
        draw_y,
        src_x, 0, PLAYER_SPRITE_WIDTH, PLAYER_SPRITE_HEIGHT,
        flip, zoom
    );

    float time_sec = sceKernelGetSystemTimeWide() / 1000000.0f;
    float offset = sinf(time_sec * 6.0f) * 3.0f;
    
    vita2d_draw_texture_part_scale(texture_dialogue_interaction, 
        posx + PLAYER_REEL_WIDTH * zoom - 2 * zoom,
        posy - 5 * zoom + offset,
        0, 0, 9, 9,
        zoom, zoom
    );

    // Rect boundingBox = getBoundingBox();
    // vita2d_draw_rectangle(
    //     boundingBox.get_position_x() * zoom - Rect::get_map_pos_x(),
    //     boundingBox.get_position_y() * zoom - Rect::get_map_pos_y(),
    //     boundingBox.get_width() * zoom,
    //     boundingBox.get_height() * zoom,
    //     RGBA8(0, 255, 0, 128) // Semi-transparent green for the bounding box
    // );
}
