#include "include/objects/player.hpp" 
#include "include/map.hpp"  

Player::Player(int x, int y) : Object(Rect(x, y, PLAYER_REEL_WIDTH*Rect::get_zoom(), PLAYER_REEL_HEIGHT*Rect::get_zoom(),false), ObjectType::PLAYER, 1), speed(3) {
    direction = {RIGHT, DOWN};
    init_all_states();
    current_state = all_states[0];
}

Player::~Player() {
    for (auto& state : all_states) {
        vita2d_free_texture(state.texture_base);
        vita2d_free_texture(state.texture_hair);
        vita2d_free_texture(state.texture_tools);
    }
    vita2d_free_pgf(font);
}

void Player::init_all_states(){
    PlayerStateData idle(PlayerState::IDLE, 8, 0.2f);
    idle.texture_base = vita2d_load_PNG_buffer(&_binary_assets_Characters_Idle_base_png_start);
    idle.texture_hair = vita2d_load_PNG_buffer(&_binary_assets_Characters_Idle_bowlhair_png_start);
    idle.texture_tools = vita2d_load_PNG_buffer(&_binary_assets_Characters_Idle_tools_png_start);

    PlayerStateData walk(PlayerState::WALK, 8, 0.05f);
    walk.texture_base = vita2d_load_PNG_buffer(&_binary_assets_Characters_Walk_base_png_start);
    walk.texture_hair = vita2d_load_PNG_buffer(&_binary_assets_Characters_Walk_bowlhair_png_start);
    walk.texture_tools = vita2d_load_PNG_buffer(&_binary_assets_Characters_Walk_tools_png_start);

    PlayerStateData axe(PlayerState::AXE, 10, 0.07f, true,7);
    axe.texture_base = vita2d_load_PNG_buffer(&_binary_assets_Characters_Axe_base_png_start);
    axe.texture_hair = vita2d_load_PNG_buffer(&_binary_assets_Characters_Axe_bowlhair_png_start);
    axe.texture_tools = vita2d_load_PNG_buffer(&_binary_assets_Characters_Axe_tools_png_start);

    PlayerStateData swimming(PlayerState::SWIMMING, 12, 0.08f);
    swimming.texture_base = vita2d_load_PNG_buffer(&_binary_assets_Characters_Swimming_base_png_start);
    swimming.texture_hair = vita2d_load_PNG_buffer(&_binary_assets_Characters_Swimming_bowlhair_png_start);
    swimming.texture_tools = vita2d_load_PNG_buffer(&_binary_assets_Characters_Swimming_tools_png_start);

    all_states.push_back(idle);
    all_states.push_back(walk);
    all_states.push_back(axe);
    all_states.push_back(swimming);
}

void Player::draw(){
    int src_x = frame * PLAYER_SPRITE_WIDTH;
    float zoom = Rect::get_zoom();
    int posx = get_position_x();
    int posy = get_position_y();

    float flip = (get_direction_x() == LEFT) ? -zoom : zoom;

    float draw_x = (get_direction_x() == LEFT) ? posx - PLAYER_REEL_POS_X*zoom + (PLAYER_SPRITE_WIDTH * zoom) + 3 : posx - PLAYER_REEL_POS_X*zoom;
    float draw_y = posy - PLAYER_REEL_POS_Y*zoom;

    vita2d_draw_texture_part_scale(current_state.texture_base, draw_x, draw_y, 
                                       src_x, 0, PLAYER_SPRITE_WIDTH, PLAYER_SPRITE_HEIGHT, 
                                       flip, zoom);
    

    vita2d_draw_texture_part_scale(current_state.texture_hair, draw_x, draw_y, 
                                   src_x, 0, PLAYER_SPRITE_WIDTH, PLAYER_SPRITE_HEIGHT, 
                                   flip, zoom);

    vita2d_draw_texture_part_scale(current_state.texture_tools, draw_x, draw_y, 
                                   src_x, 0, PLAYER_SPRITE_WIDTH, PLAYER_SPRITE_HEIGHT, 
                                   flip, zoom);
    

    if (debug) {
        Console::log("Player position {X,Y} : {" + std::to_string(posx) + " , " + std::to_string(posy)+"}");
        if(current_state.state == WALK){
            Console::log("Player state : WALK");
        }else{
            Console::log("Player state : IDLE");
        }

        auto [left, right, top, bottom] = get_position();
        Console::log("Player rect {left, right, top, bottom} : {" + std::to_string(left) + " , " + std::to_string(right) + " , " + std::to_string(top) + " , " + std::to_string(bottom) + "}");
        vita2d_draw_line(left, top, right, top, RGBA8(255, 0, 0, 255));
        vita2d_draw_line(left, top, left, bottom, RGBA8(255, 0, 0, 255));
        vita2d_draw_line(right, top, right, bottom, RGBA8(255, 0, 0, 255));
        vita2d_draw_line(left, bottom, right, bottom, RGBA8(255, 0, 0, 255));
        
        //afficher la zone de collision avec #define COLLIDE_ZONE_Y 
        vita2d_draw_rectangle(posx, posy + (PLAYER_REEL_HEIGHT - COLLIDE_ZONE_Y)*zoom,
        PLAYER_REEL_WIDTH * zoom, COLLIDE_ZONE_Y * zoom,RGBA8(255, 0, 0, 255)); 
    }
}



void Player::update(){
    if(current_state.nb_frame==0) return;
    time+=1.0f / 60.0f;
    if(time>current_state.frame_duration){
        time=0;
        frame++;
        if(frame>=current_state.nb_frame){
            if(current_state.blocking){
                if(current_state.state==AXE){
                    change(IDLE);
                }else{
                    change(WALK);  
                }
            }
            frame=0;
        }
    }
}

void Player::interaction(){
    // no interaction
}

void Player::move(int x, int y) {
    int zoom = Rect::get_zoom();
    //Console::log("get_position_x() + x : " + std::to_string(get_position_x() + x) + " SCREEN_WIDTH - PLAYER_REEL_WIDTH * zoom : " + std::to_string(SCREEN_WIDTH - PLAYER_REEL_WIDTH * zoom));
    if(get_position_x() + x <= 0){
        x = -get_position_x();
    }else if (get_position_x() + x >= SCREEN_WIDTH - PLAYER_REEL_WIDTH * zoom){
        x = SCREEN_WIDTH - PLAYER_REEL_WIDTH * zoom - get_position_x();
    }
    
    if(get_position_y() + y <= 0){
        y = -get_position_y();
    }else if (get_position_y() + y >= SCREEN_HEIGHT - PLAYER_REEL_HEIGHT * zoom){
        y = SCREEN_HEIGHT - PLAYER_REEL_HEIGHT * zoom - get_position_y();
    }
    
    set_position(x,y);
}

void Player::change(PlayerState state){
    if(current_state.blocking && frame!=current_state.nb_frame) return;
    if(all_states[state].blocking || current_state.state != state) frame=0;
    current_state = all_states[state];
}

Rect Player::get_rect_collision() const{
    return Rect(get_position_x(), get_position_y() + (PLAYER_REEL_HEIGHT - COLLIDE_ZONE_Y)*Rect::get_zoom(), PLAYER_REEL_WIDTH*Rect::get_zoom(), COLLIDE_ZONE_Y*Rect::get_zoom(),false);
}

void Player::update_cutscene(float deltaTime, Map& map) {
    if (!cutscene.active || cutscene.currentStep >= cutscene.steps.size())
        return;

    CutsceneStep& step = cutscene.steps[cutscene.currentStep];
    cutscene.timer += deltaTime;

    switch (step.type) {
        case CutsceneStepType::Move: {
            if (cutscene.timer == deltaTime) {
                cutscene.alreadyMoved = {0.0f, 0.0f};
            }

            float t = cutscene.timer / step.duration;
            if (t > 1.0f) t = 1.0f;

            float expectedTotalX = step.direction.first * t;
            float expectedTotalY = step.direction.second * t;

            float deltaX = expectedTotalX - cutscene.alreadyMoved.first;
            float deltaY = expectedTotalY - cutscene.alreadyMoved.second;

            int moveX = 0;
            int moveY = 0;

            // Seulement bouger si la fraction accumulée dépasse ±1.0
            if (std::abs(deltaX) >= 1.0f) {
                moveX = static_cast<int>(std::round(deltaX));
                cutscene.alreadyMoved.first += moveX;
            }

            if (std::abs(deltaY) >= 1.0f) {
                moveY = static_cast<int>(std::round(deltaY));
                cutscene.alreadyMoved.second += moveY;
            }

            if (moveX != 0 || moveY != 0) {
                // move(moveX, moveY);
                map.move(moveX, moveY);
            }

            change(PlayerState::WALK);
            // modifier la direction du joueur
            if (moveX > 0) {
                set_direction_x(RIGHT);
            } else if (moveX < 0) {
                set_direction_x(LEFT);
            }

            if (cutscene.timer >= step.duration) {
                // Fin du mouvement, on s'assure d'arriver exactement à la bonne position
                int targetX = get_position_x() + (step.direction.first - static_cast<int>(cutscene.alreadyMoved.first));
                int targetY = get_position_y() + (step.direction.second - static_cast<int>(cutscene.alreadyMoved.second));
                move(targetX - get_position_x(), targetY - get_position_y());

                change(PlayerState::IDLE);
                cutscene.timer = 0.0f;
                cutscene.currentStep++;
            }

            break;
        }

        case CutsceneStepType::Wait: {
            change(PlayerState::IDLE);
            if (cutscene.timer >= step.waitTime) {
                cutscene.timer = 0.0f;
                cutscene.currentStep++;
            }
            break;
        }

        case CutsceneStepType::Dialogue: {
            // Tu peux intégrer ton système de dialogue ici           
            break;
        }

        case CutsceneStepType::Fade: {
            // à gérer dans ton système de transition (ex. fade-in/out)
            if (cutscene.timer >= step.duration) {
                cutscene.timer = 0.0f;
                cutscene.currentStep++;
            }
            break;
        }

        case CutsceneStepType::Callback: {
            if (step.callback) step.callback();
            cutscene.timer = 0.0f;
            cutscene.currentStep++;
            break;
        }
    }

    if (cutscene.currentStep >= cutscene.steps.size()) {
        cutscene.active = false;
        change(PlayerState::IDLE);
    }
}
