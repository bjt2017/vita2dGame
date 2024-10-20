#include "include/player.hpp"



Player::Player(int x, int y, int speed_value) {
    posx = x;
    posy = y;

    posx_player = posx + PLAYER_REEL_POS_X*ZOOM;
    posy_player = posy + PLAYER_REEL_POS_Y*ZOOM;

    current_layer = 1;
    speed = speed_value;
    frame = 0;
    frame_duration = 0.1f;  
    animation_timer = 0.0f;
    direction_x = RIGHT;
    direction_y = DOWN; 
    state = IDLE; 
    
    idle.texture_base = vita2d_load_PNG_buffer(&_binary_assets_Characters_Idle_base_png_start);
    idle.texture_hair = vita2d_load_PNG_buffer(&_binary_assets_Characters_Idle_bowlhair_png_start);
    idle.texture_tools = vita2d_load_PNG_buffer(&_binary_assets_Characters_Idle_tools_png_start);

    walk.texture_base = vita2d_load_PNG_buffer(&_binary_assets_Characters_Walk_base_png_start);
    walk.texture_hair = vita2d_load_PNG_buffer(&_binary_assets_Characters_Walk_bowlhair_png_start);
    walk.texture_tools = vita2d_load_PNG_buffer(&_binary_assets_Characters_Walk_tools_png_start);

    axe.texture_base = vita2d_load_PNG_buffer(&_binary_assets_Characters_Axe_base_png_start);
    axe.texture_hair = vita2d_load_PNG_buffer(&_binary_assets_Characters_Axe_bowlhair_png_start);
    axe.texture_tools = vita2d_load_PNG_buffer(&_binary_assets_Characters_Axe_tools_png_start);
    axe.blocking = true;
    axe.nb_frame = 10;
    axe.can_move = false;

    current_state = idle;

    font = vita2d_load_default_pgf();
    debug = false;
}

void free_texture(vita2d_texture* texture) {
    if (texture) {
        vita2d_free_texture(texture);
    }
}

Player::~Player() {
    free_texture(idle.texture_base);
    free_texture(idle.texture_hair);
    free_texture(idle.texture_tools);

    free_texture(walk.texture_base);
    free_texture(walk.texture_hair);
    free_texture(walk.texture_tools);

    vita2d_free_pgf(font);
}

void Player::draw() {

    int src_x = frame * PLAYER_SPRITE_WIDTH;
    float flip = (direction_x == LEFT) ? -ZOOM : ZOOM;
    float draw_x = (direction_x == LEFT) ? posx + (PLAYER_SPRITE_WIDTH * ZOOM) + 1 : posx;


    vita2d_draw_texture_part_scale(current_state.texture_base, draw_x, posy, 
                                       src_x, 0, PLAYER_SPRITE_WIDTH, PLAYER_SPRITE_HEIGHT, 
                                       flip, ZOOM);
    

    vita2d_draw_texture_part_scale(current_state.texture_hair, draw_x, posy, 
                                   src_x, 0, PLAYER_SPRITE_WIDTH, PLAYER_SPRITE_HEIGHT, 
                                   flip, ZOOM);

    vita2d_draw_texture_part_scale(current_state.texture_tools, draw_x, posy, 
                                   src_x, 0, PLAYER_SPRITE_WIDTH, PLAYER_SPRITE_HEIGHT, 
                                   flip, ZOOM);
    

    if (debug) {
        console.log("Player position {X,Y} : {" + std::to_string(posx) + " , " + std::to_string(posy)+"}");
        
        if(state == WALK){
            console.log("Player state : WALK");
        }else{
            console.log("Player state : IDLE");
        }

        //affichier la zone du joueur 
        vita2d_draw_line(posx_player, posy_player, posx_player + PLAYER_REEL_WIDTH * ZOOM, posy_player, RGBA8(255, 0, 0, 255));
        vita2d_draw_line(posx_player, posy_player, posx_player, posy_player + PLAYER_REEL_HEIGHT * ZOOM, RGBA8(255, 0, 0, 255)); 
        vita2d_draw_line(posx_player + PLAYER_REEL_WIDTH * ZOOM, posy_player, posx_player + PLAYER_REEL_WIDTH * ZOOM, posy_player + PLAYER_REEL_HEIGHT * ZOOM, RGBA8(255, 0, 0, 255)); 
        vita2d_draw_line(posx_player, posy_player + PLAYER_REEL_HEIGHT * ZOOM, posx_player + PLAYER_REEL_WIDTH * ZOOM, posy_player + PLAYER_REEL_HEIGHT * ZOOM, RGBA8(255, 0, 0, 255)); 
        
        //afficher la zone de collision avec #define COLLIDE_ZONE_Y 
        vita2d_draw_rectangle(posx_player, posy_player + (PLAYER_REEL_HEIGHT - COLLIDE_ZONE_Y)*ZOOM, 
         PLAYER_REEL_WIDTH * ZOOM, COLLIDE_ZONE_Y * ZOOM,
          RGBA8(255, 0, 0, 255));
    }

}

void Player::move(int x, int y) {
    if(posx_player + x <= 0){
        x = -posx_player;
    }else if (posx_player + x >= SCREEN_WIDTH - PLAYER_REEL_WIDTH * ZOOM){
        x = SCREEN_WIDTH - PLAYER_REEL_WIDTH * ZOOM - posx_player;
    }
    
    if(posy_player + y <= 0){
        y = -posy_player;
    }else if (posy_player + y >= SCREEN_HEIGHT - PLAYER_REEL_HEIGHT * ZOOM){
        y = SCREEN_HEIGHT - PLAYER_REEL_HEIGHT * ZOOM - posy_player;
    }
    
    posx += x;
    posy += y;

    posx_player = posx + PLAYER_REEL_POS_X*ZOOM;
    posy_player = posy + PLAYER_REEL_POS_Y*ZOOM;
}

void Player::changeState(int new_state) {

    if (state != new_state && !current_state.blocking) {
        state = new_state;
        frame = 0; 
        if (state == IDLE) {
            current_state = idle;
        } else if (state == WALK) {
            current_state = walk;
        } else if (state == Axe) {
            current_state = axe;
            animation_timer = 0.0f;
        }
    }
}


void Player::update_animation(float delta_time) {
    animation_timer += delta_time;

    // Si l'animation a atteint sa durée pour une frame, passer à la suivante
    if (animation_timer >= frame_duration) {
        frame++;

        if (current_state.blocking && frame >= current_state.nb_frame) {
            frame = 0;
            current_state = idle;
            state = IDLE;
        } else {
            frame = frame % current_state.nb_frame;
        }

        animation_timer = 0.0f;
    }
}

bool Player::collision_bounding_box(std::tuple<int, int, int, int> boundingBox, Map* map) {
    auto object_x = std::get<0>(boundingBox) - map->posX; 
    auto object_y = std::get<1>(boundingBox) - map->posY;
    auto object_width = std::get<2>(boundingBox);
    auto object_height = std::get<3>(boundingBox);

    int player_top = posy_player + (PLAYER_REEL_HEIGHT - COLLIDE_ZONE_Y) * ZOOM;
    int player_right = posx_player + PLAYER_REEL_WIDTH * ZOOM;
    int player_bottom = posy_player + PLAYER_REEL_HEIGHT * ZOOM;
    int player_left = posx_player;

    bool collisionX = (player_left < object_x + object_width) && (player_right > object_x);
    bool collisionY = (player_top < object_y + object_height) && (player_bottom > object_y);

    return collisionX && collisionY;
}



std::pair<int, int> Player::collision_bounding_box(std::tuple<int, int,int,int> boundingBox, Map *map, int x, int y){
    auto object_x = std::get<0>(boundingBox);
    auto object_y = std::get<1>(boundingBox);
    auto object_width = std::get<2>(boundingBox);
    auto object_height = std::get<3>(boundingBox);

    int maxX = x;
    int maxY = y;

    // Calcul des limites du joueur
    int player_top = posy_player + (PLAYER_REEL_HEIGHT - COLLIDE_ZONE_Y)*ZOOM;
    int player_right = posx_player + PLAYER_REEL_WIDTH * ZOOM;
    int player_bottom = posy_player + PLAYER_REEL_HEIGHT * ZOOM;

    // Vérification de la collision en X (mouvement horizontal)
    if (player_bottom > object_y && player_top < object_y + object_height) { // Collision possible en X
        if (x > 0 && player_right + x > object_x && posx_player + x < object_x + object_width) { // Se déplace vers la droite
            maxX = std::min(maxX, object_x - player_right);
        } else if (x < 0 && posx_player + x < object_x + object_width && player_right + x > object_x) { // Se déplace vers la gauche
            maxX = std::max(maxX, object_x + object_width - posx_player);
        }
    }

    // Vérification de la collision en Y (mouvement vertical)
    if (player_right + maxX > object_x && posx_player + maxX < object_x + object_width) { // Collision possible en Y
        if (y > 0 && player_bottom + y > object_y && player_top + y < object_y + object_height) { // Se déplace vers le bas
            maxY = std::min(maxY, object_y - player_bottom);
        } else if (y < 0 && player_top + y < object_y + object_height && player_bottom + y > object_y) { // Se déplace vers le haut
            maxY = std::max(maxY, object_y + object_height - player_top);
        }
    }

    return {maxX, maxY}; // Retourne la distance ajustée maximale en X et Y
}

std::pair<int, int> Player::collision_rect(tmx::Object object, Map *map, int x, int y) {
    auto obj = getBoundingBox(object, ZOOM, map->posX, map->posY);
    return collision_bounding_box(obj, map, x,y);    
}

std::pair<int, int> Player::collision_polygon(tmx::Object object, Map *map, int x, int y) {
    auto obj = getBoundingBox(object, ZOOM, map->posX, map->posY);
    return collision_bounding_box(obj, map, x,y);   
}


std::tuple<int, int, int, int> Player::getBoundingBox(const tmx::Object& object, float zoom, int mapPosX, int mapPosY) {
    int object_x, object_y, object_width, object_height;

    if (object.getShape() == tmx::Object::Shape::Rectangle) {
        // Récupération des coordonnées et dimensions du rectangle
        object_x = object.getAABB().left * zoom - mapPosX;
        object_y = object.getAABB().top * zoom - mapPosY;
        object_width = object.getAABB().width * zoom;
        object_height = object.getAABB().height * zoom;
    } else if (object.getShape() == tmx::Object::Shape::Polygon || object.getShape() == tmx::Object::Shape::Polyline) {
        const auto& points = object.getPoints();
        if (points.size() < 2) return {0, 0, 0, 0}; // Pas assez de points pour créer une bounding box

        
        const auto& position = object.getPosition();
        int minX = points[0].x, maxX = points[0].x;
        int minY = points[0].y, maxY = points[0].y;
        for (const auto& point : points) {
            if (point.x < minX) minX = point.x;
            if (point.x > maxX) maxX = point.x;
            if (point.y < minY) minY = point.y;
            if (point.y > maxY) maxY = point.y;
        }


        object_x = (position.x + minX) * zoom - mapPosX;
        object_y = (position.y + minY) * zoom - mapPosY;
        
        object_width = (maxX - minX) * zoom;
        object_height = (maxY - minY) * zoom;
    } else {
        return {0, 0, 0, 0}; 
    }

    return {object_x, object_y, object_width, object_height}; // Retourne 4 valeurs
}