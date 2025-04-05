#include "include/main.h"
#include <psp2/appmgr.h>
#include <GLES2/gl2.h>
#include <vita2d.h>
#include <psp2/power.h>
#include <psp2/kernel/modulemgr.h>

void move(SceCtrlData &pad, Player &player, Map &map);
void draw(Player &player, Map &map);
void update(Player &player);

float ZOOM = 3.0f;

std::vector<Rect> list_collide_rect = {}; 
std::vector<Tree> list_tree = {};  

std::vector<Rect*> list_collide_rect_on_screen = {}; 
std::vector<Tree*> list_tree_on_screen = {};  

int main() {

    SceCtrlData pad, previousPad;
    SceTouchData touch_data;

    // Initialisation Vita2D
    vita2d_init();
    vita2d_set_clear_color(RGBA8(0x40, 0x40, 0x40, 0xFF));

    Map map;
    // Créer un joueur au centre de l'écran
    Player player(SCREEN_WIDTH / 2 - (PLAYER_REEL_WIDTH*ZOOM) / 2, SCREEN_HEIGHT / 2 - (PLAYER_REEL_HEIGHT*ZOOM) / 2);

    map.init(player);

    Console::init();

    // Initialisation du touchpad
    sceTouchSetSamplingState(SCE_TOUCH_PORT_FRONT, SCE_TOUCH_SAMPLING_STATE_START);
    sceTouchEnableTouchForce(SCE_TOUCH_PORT_FRONT);

    sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG);


    // Initialisation du pad
    memset(&pad, 0, sizeof(pad));
    memset(&previousPad, 0, sizeof(previousPad));  // Initialiser le pad précédent à 0

    // Boucle principale du jeu
    while (1) {
        sceCtrlPeekBufferPositive(0, &pad, 1);

        sceTouchPeek(SCE_TOUCH_PORT_FRONT, &touch_data, 1);

        if (pad.buttons & SCE_CTRL_START)
            break;

        if ((pad.buttons & SCE_CTRL_SELECT) && !(previousPad.buttons & SCE_CTRL_SELECT)) {
            player.debug = !player.debug;
        } 
        for (SceUInt32 i = 0; i < touch_data.reportNum; i++) {
            if (touch_data.report[i].x != 0 && touch_data.report[i].y != 0) {
                Console::log("Touch detected at {X,Y} : {" + std::to_string(touch_data.report[i].x) + " , " + std::to_string(touch_data.report[i].y) + "}");
            }
        }
        if (touch_data.reportNum == 0) {
            Console::log("No touch detected");
        }

        vita2d_start_drawing();
        vita2d_clear_screen();

        //update player position and animation
        update(player);
        move(pad, player, map);
        Utils::update_list();
        draw(player, map);

        if (player.debug) {
            Console::log("Nombre d'arbres : " + std::to_string(list_tree.size()));
            Console::log("Nombre d'arbres sur l'écran : " + std::to_string(list_tree_on_screen.size()));

            if ((pad.buttons & SCE_CTRL_TRIANGLE) && !(previousPad.buttons & SCE_CTRL_TRIANGLE)) {
                Console::mode = Console::mode == 0 ? 1 : 0;
            }
            for (const auto& rect : list_collide_rect) {
                if (rect.getShape() == tmx::Object::Shape::Rectangle) {
                    // Dessin d'un rectangle
                    vita2d_draw_rectangle(
                        rect.get_position_x() * ZOOM - map.posX,
                        rect.get_position_y() * ZOOM - map.posY,
                        rect.get_width() * ZOOM,
                        rect.get_height() * ZOOM,
                        RGBA8(255, 0, 0, 255)
                    );
                } else if (rect.getShape() == tmx::Object::Shape::Polygon) {
                    /*const auto& points = object.getPoints();
                    const auto& position = object.getPosition();

                    if (points.size() < 3) continue;

                    for (size_t i = 0; i < points.size(); ++i) {
                        float x1 = (position.x + points[i].x) * ZOOM - map.posX;
                        float y1 = (position.y + points[i].y) * ZOOM - map.posY;

                        float x2 = (position.x + points[(i + 1) % points.size()].x) * ZOOM - map.posX;
                        float y2 = (position.y + points[(i + 1) % points.size()].y) * ZOOM - map.posY;

                        vita2d_draw_line(
                            x1, y1, 
                            x2, y2, 
                            RGBA8(255, 0, 0, 255)
                        );
                    }*/
                }
                
            }
            vita2d_draw_line(SCREEN_WIDTH / 2, 0, SCREEN_WIDTH / 2, SCREEN_HEIGHT, RGBA8(255, 0, 0, 255));
            vita2d_draw_line(0, SCREEN_HEIGHT / 2, SCREEN_WIDTH, SCREEN_HEIGHT / 2, RGBA8(255, 0, 0, 255));
            Console::show();
        }

        vita2d_end_drawing();
        vita2d_swap_buffers();
        Console::clear();
        previousPad = pad;
    }

    Console::shutdown();
    vita2d_fini();
    sceKernelExitProcess(0);
    return 0;
}

void update(Player &player) {
    player.update();
    Map::update();
    for(Tree &tree : list_tree){
        tree.update();
    }
}

void draw(Player &player, Map &map)
{
    const auto& layers = map.map.getLayers();
    bool is_player_draw = false;

    for (const auto& layer : layers) {
        if (layer->getType() == tmx::Layer::Type::Tile) {
            const tmx::TileLayer& tile_layer = static_cast<const tmx::TileLayer&>(*layer);

            std::string name = layer->getName();
            
            std::string layerStr = Utils::split(name, '_', 1);
            if(layerStr.empty()) {
                Console::warning("Layer name is empty");
            }
            int actual_layer = layerStr.empty() ? 1 : std::stoi(layerStr);

            std::string layer_name = Utils::split(name, '_', 0);

            LayerType layer_type = LayerType::TILE;
            if (layer_name == "arbre") {
                layer_type = LayerType::TREE;
            } else if (layer_name == "water") {
                layer_type = LayerType::WATER;
            }

            bool draw_player = !is_player_draw && actual_layer == player.get_layer() && Utils::split(name, '_', 2) == "over";
            
            if (draw_player) {
                player.draw();
                is_player_draw = true;
            }
            
            map.draw_layer(tile_layer,layer_type);
        }
    }

    for(Tree &tree : list_tree){
        if(tree.on_screen()){
            tree.draw();
        }
    }
}



void move(SceCtrlData &pad, Player &player, Map &map) {
    int x = 0, y = 0;

    if(pad.buttons & SCE_CTRL_SQUARE){
        player.change(AXE);
    }
    if(!player.get_current_state().blocking){
        if(pad.buttons & SCE_CTRL_UP){
            y = floor(-PLAYER_SPEED * ZOOM);
            player.set_direction_y(UP);
        } else if(pad.buttons & SCE_CTRL_DOWN){
            y = ceil(PLAYER_SPEED * ZOOM);
            player.set_direction_y(DOWN);
        }
        if(pad.buttons & SCE_CTRL_LEFT){
            x = floor(-PLAYER_SPEED * ZOOM);
            player.set_direction_x(LEFT);
        } else if(pad.buttons & SCE_CTRL_RIGHT){
            x = ceil(PLAYER_SPEED * ZOOM);
            player.set_direction_x(RIGHT);
        }
        int deadzone = 35; 

        Console::log("Pad LX : " + std::to_string(pad.lx));
        Console::log("Pad LY : " + std::to_string(pad.ly));

        if(abs(pad.lx - 128) > deadzone) { 
            x = ((pad.lx - 128) / 128.0f) * PLAYER_SPEED * ZOOM;
            player.set_direction_x((x > 0) ? RIGHT : LEFT);
        }

        if(abs(pad.ly - 128) > deadzone) { 
            y = ((pad.ly - 128) / 128.0f) * PLAYER_SPEED * ZOOM;
            player.set_direction_y((y > 0) ? DOWN : UP);
        }
    }

    if(x==0 && y==0){
        player.change(IDLE);
    }

    bool map_can_move_left = map.posX + x <= (map.mapWidth * map.tileWidth * ZOOM) - SCREEN_WIDTH;
    bool map_can_move_right = map.posX + x >= 0;

    bool map_can_move_up = map.posY + y <= (map.mapHeight * map.tileHeight * ZOOM) - SCREEN_HEIGHT;
    bool map_can_move_down = map.posY + y >= 0;

    int maxX = x;
    int maxY = y;

    for (Rect rect : list_collide_rect) {
        int collideX;
        int collideY;
        if(rect.getShape() == tmx::Object::Shape::Rectangle){
            auto collisionResult = player.get_rect_collision().collision(rect, x, y);
            collideX = collisionResult.first;
            collideY = collisionResult.second;
            
        } else if (rect.getShape() == tmx::Object::Shape::Polygon){
            //TODO
        }
        maxX = (x > 0) ? std::min(maxX, collideX) : std::max(maxX, collideX);
        maxY = (y > 0) ? std::min(maxY, collideY) : std::max(maxY, collideY);
    }

    for (Tree &tree : list_tree){
        if(tree.on_screen()){
            const bool can_interact = tree.can_interact(player.get_rect(), player.get_direction_x() == RIGHT);
            if (can_interact) {
                //console.log("current_state : " + player.current_state.state + " frame : " + std::to_string(player.frame) + " action_frame : " + std::to_string(player.current_state.action_frame));
                if(player.get_current_state().state == AXE && player.get_frame()==player.get_current_state().action_frame-1){
                    tree.interaction();
                }
                else{
                    tree.change(SELECT);
                }
            } else {
                tree.change(NORMAL);
            }
        }
    }

    if (maxX != 0 && maxY != 0 && maxX==x && maxY==y){
        float magnitude = sqrt(maxX * maxX + maxY * maxY);
        if (magnitude > PLAYER_SPEED * ZOOM) {
            float normalizationFactor = (PLAYER_SPEED * ZOOM) / magnitude;
            maxX = static_cast<int>(maxX * normalizationFactor);
            maxY = static_cast<int>(maxY * normalizationFactor);
        }
    }

    Console::log("maxX : " + std::to_string(maxX) + " maxY : " + std::to_string(maxY));

    if(map_can_move_left && player.get_direction_x() == RIGHT && player.get_position_x() == player_center_x){
        map.move(maxX, 0);
    } else if(player.get_direction_x() == RIGHT){
        if(player.get_position_x() < player_center_x && player.get_position_x() + maxX > player_center_x){
            maxX = player_center_x - player.get_position_x();
        }
        player.move(maxX, 0);
    }
    else if(map_can_move_right && player.get_direction_x() == LEFT && player.get_position_x() == player_center_x){
        map.move(maxX, 0);
    } else if(player.get_direction_x() == LEFT){
        if(player.get_position_x() > player_center_x && player.get_position_x() + maxX < player_center_x){
            maxX = player_center_x - player.get_position_x();
        }
        player.move(maxX, 0);
    }

    if(map_can_move_up && player.get_direction_y() == DOWN && player.get_position_y() == player_center_y){
        map.move(0, maxY);
    } else if(player.get_direction_y() == DOWN){
        if(player.get_position_y() < player_center_y && player.get_position_y() + maxY > player_center_y){
            maxY = player_center_y - player.get_position_y();
        }
        player.move(0, maxY);
    }
    else if(map_can_move_down && player.get_direction_y() == UP && player.get_position_y() == player_center_y){
        map.move(0, maxY);
    }
    else if(player.get_direction_y() == UP){
        if(player.get_position_y() > player_center_y && player.get_position_y() + maxY < player_center_y){
            maxY = player_center_y - player.get_position_y();
        }
        player.move(0, maxY);
    }

    player.change((maxX == 0 && maxY == 0) ? IDLE : WALK);
   
}