#include "include/main.h"
#include <psp2/appmgr.h>
#include <GLES2/gl2.h>
#include <vita2d.h>
#include <psp2/power.h>
#include <psp2/kernel/modulemgr.h>

void move(SceCtrlData &pad, Player &player, Map &map);
void draw(Player &player, Map &map);
void update(Player &player, Map &map, const SceCtrlData &pad, const SceCtrlData &previousPad);
void show_debug_info(const Map& map,const SceCtrlData& pad,const SceCtrlData& previousPad);

float ZOOM = 3.0f;

std::unordered_map<int, std::vector<Shape>> list_collide_rect_by_layer = {};
std::vector<Shape> list_collide_rect_global = {};
std::vector<Shape*> list_collide_rect = {};

std::vector<std::unique_ptr<Portal>> list_portal;
std::unordered_map<std::string, std::vector<Tree>> list_tree_by_map = {};
std::vector<Tree>* list_tree = nullptr;

std::vector<Character> list_character = {};

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
    UIBox::load_assets();

    Character::load_assets();
    list_character.push_back(Character({217, 330}, "Test", "Character Description"));

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

        if (pad.buttons & SCE_CTRL_START) break;
        if ((pad.buttons & SCE_CTRL_SELECT) && !(previousPad.buttons & SCE_CTRL_SELECT)) {
            player.debug = !player.debug;
        } 
        for (SceUInt32 i = 0; i < touch_data.reportNum; i++) {
            if (touch_data.report[i].x != 0 && touch_data.report[i].y != 0) {
                Console::log("Touch detected at {X,Y} : {" + std::to_string(touch_data.report[i].x) + " , " + std::to_string(touch_data.report[i].y) + "}");
            }
        }
        if (touch_data.reportNum == 0) {
            Console::log(u8"No touch detected");
        }

        // Démarrer le render et effacer l'écran
        vita2d_start_drawing();
        vita2d_clear_screen();

        //update player position and animation
        if(map.transition.state == TransitionState::None && !dialogueManager.is_active()){
            move(pad, player, map);
        }
        update(player, map, pad, previousPad);
        draw(player, map);
        dialogueManager.update(pad, previousPad);
        dialogueManager.draw();

        // Afficher les objets de collision, les portails et les logs
        if (player.debug) {
            show_debug_info(map, pad, previousPad);
        }

        vita2d_end_drawing();
        vita2d_swap_buffers();
        Console::clear();
        previousPad = pad;
    }
    Character::free_texture();
    Tree::free_assets();
    UIBox::free_assets();
    Console::shutdown();
    vita2d_fini();
    sceKernelExitProcess(0);
    return 0;
}

void update(Player &player, Map &map, const SceCtrlData &pad, const SceCtrlData &previousPad) {
    player.update();
    if (player.is_playing_cutsense()) {
        player.update_cutscene(1.0f / 60.0f, map);
    }
    map.update(player);
    for(Tree &tree : *list_tree){
        tree.update();
    }
    for(Character &character : list_character) {
        character.update(player, pad, previousPad, dialogueManager.is_active());
    }
}

void draw(Player &player, Map &map)
{
    const auto& layers = map.getCurrentMap().getLayers();
    bool is_player_draw = false;

    //afficher toutes les layers d'eau 
    try {
        const tmx::LayerGroup& water_layers = map.get_tile_group({"water"}, false);
        
        for (const auto& layerPtr : water_layers.getLayers()) {
            if (layerPtr->getType() == tmx::Layer::Type::Tile) {
                const tmx::TileLayer& tile_layer = layerPtr->getLayerAs<tmx::TileLayer>();
                map.draw_layer(tile_layer, LayerType::WATER);
            }
        }
    } catch (const std::exception& e) {
        Console::warning("Impossible de récupérer le groupe 'water' : " + std::string(e.what()));
    }

    for (const auto& layer : layers) {
        if (layer->getType() == tmx::Layer::Type::Tile) {
            const tmx::TileLayer& tile_layer = static_cast<const tmx::TileLayer&>(*layer);
            bool draw_layer = true;
            std::string name = layer->getName();
            
            std::string layerStr = Utils::split(name, '_', 1);
            if(layerStr.empty()) {
                Console::warning("Layer name is empty");
            }
            int actual_layer = layerStr.empty() ? 1 : std::stoi(layerStr);

            std::string layer_name = Utils::split(name, '_', 0);

            std::string layer_type_str = Utils::split(name, '_', 2);

            LayerType layer_type = LayerType::TILE;
            if (layer_name == "arbre") {
                layer_type = LayerType::TREE;
            } else if (layer_name == "water") {
                layer_type = LayerType::WATER;
            } else if(layer_name == "house" && layer_type_str == "over"){
                draw_layer = player.get_interaction_house() == nullptr;
                if(!draw_layer) {
                    //afficher le nombre de tuiles du root de la house
                    Console::log("House interaction nb roof tiles: " + player.get_interaction_house()->roof_tiles_pos.size());
                }
            }

            bool draw_player = !is_player_draw && Utils::split(name, '_', 0)!="tiles" && layer_type_str != "alwaysunder" && (actual_layer == player.get_layer()+1 || layer_type_str == "over");
            
            if (draw_player) {
                const auto [player_left, player_right, player_top, player_bottom] = player.get_position();

                // 1. Draw characters above the player
                for (Character& character : list_character) {
                    if (!character.get_rect().on_screen()) continue;

                    const auto [char_left, char_right, char_top, char_bottom] = character.get_rect().get_position();
                    if (char_bottom < player_bottom) {
                        character.draw();
                    }
                }

                // 2. Draw the player
                player.draw();

                // 3. Draw characters below the player
                for (Character& character : list_character) {
                    if (!character.get_rect().on_screen()) continue;

                    const auto [char_left, char_right, char_top, char_bottom] = character.get_rect().get_position();
                    if (char_bottom >= player_bottom) {
                        character.draw();
                    }
                }

                is_player_draw = true;
            }

            if(draw_layer) map.draw_layer(tile_layer,layer_type);
        }
    }

    for(Tree &tree : *list_tree){
        if(tree.on_screen()){
            tree.draw();
        }
    }

    map.draw_transition();
}

void move(SceCtrlData &pad, Player &player, Map &map) {
    if(player.is_playing_cutsense())return;
    bool player_on_water = false;
    //verifier si le joueur est sur l'eau
    if(map.player_on_water(player)){
        player_on_water = true;
        if(player.get_current_state().state != SWIMMING){
            player.change(SWIMMING);
            player.set_can_interact(false);
        }
    } else{
        player.set_can_interact(true);
    }
    int x = 0, y = 0;

    if(player.get_can_interact()){
        if(pad.buttons & SCE_CTRL_SQUARE){
            player.change(AXE);
        }
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

    bool map_can_move_left = map.posX + x <= (map.mapWidth * map.tileWidth * ZOOM) - SCREEN_WIDTH;
    bool map_can_move_right = map.posX + x >= 0;

    bool map_can_move_up = map.posY + y <= (map.mapHeight * map.tileHeight * ZOOM) - SCREEN_HEIGHT;
    bool map_can_move_down = map.posY + y >= 0;

    int maxX = x;
    int maxY = y;

    // Traitement des collisions
    for (const Shape* shape : list_collide_rect) {
        std::visit([&](const auto& obj) {
            using T = std::decay_t<decltype(obj)>;
            int collideX = 0;
            int collideY = 0;

            if constexpr (std::is_same_v<T, Rect>) {
                auto collisionResult = player.get_rect_collision().collision(obj, x, y);
                collideX = collisionResult.first;
                collideY = collisionResult.second;
            } else if constexpr (std::is_same_v<T, Polygon>) {
                auto collisionResult = player.get_rect_collision().collision(obj, x, y);
                collideX = collisionResult.first;
                collideY = collisionResult.second;
            }

            maxX = (x > 0) ? std::min(maxX, collideX) : std::max(maxX, collideX);
            maxY = (y > 0) ? std::min(maxY, collideY) : std::max(maxY, collideY);
        }, *shape);  // <- ici on déréférence le pointeur
    }


    //afficher les differentes map qui existent dans list_tree_by_map
    std::string list_tree_by_map_str = "List of maps with trees: ";
    for (const auto& pair : list_tree_by_map) {
        list_tree_by_map_str += pair.first+ ", nb :" + std::to_string(pair.second.size()) + ", ";
    }
    Console::log(list_tree_by_map_str);
    Console::log("current map" + map.current_map_name);


    for (Tree &tree : *list_tree){
        if(tree.get_layer()==player.get_layer() && tree.on_screen()){
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
    if(!player_on_water){
        player.change((maxX == 0 && maxY == 0) ? IDLE : WALK);
    }
    player.last_direction = {maxX, maxY};

    Console::log("nb portals : " + std::to_string(list_portal.size()));
    // verifier les portails
    // si le joueur etait sur un portail, on verifie si il est plus dedans
    bool player_changed_layer = false;
    std::string new_map_name = "null";
    Portal *interaction_portal = player.get_interaction_portal();
    if(interaction_portal != nullptr){
        Console::log("Player is in a portal and he enters in the portal by " + std::to_string(static_cast<int>(player.get_portal_entry_direction())));
        if(!player.get_rect_collision().intersects(player.get_interaction_portal()->rect)){
            // le joueur n'est plus dans le portail
            if(player.get_portal_entry_direction() == Position::Top && player.get_rect_collision().get_position_relative(player.get_interaction_portal()->rect).first == Position::Bottom){
                //si il est sorti par le bas on decremente la couche sur la quelle le joueur est
                if(interaction_portal->type == PortalType::LayerChanger){
                    player.update_layer(-1);
                    player_changed_layer = true;
                } else if(interaction_portal->type == PortalType::House){
                    player.set_interaction_house(nullptr);
                }
            } else if(player.get_portal_entry_direction() == Position::Bottom && player.get_rect_collision().get_position_relative(player.get_interaction_portal()->rect).first == Position::Top){
                if(interaction_portal->type == PortalType::LayerChanger){
                    player.update_layer(1);
                    player_changed_layer = true;
                } else if(interaction_portal->type == PortalType::House){
                    player.set_interaction_house(dynamic_cast<House*>(interaction_portal));
                }
            }
            player.set_interaction_portal(nullptr);
            player.set_portal_entry_direction(Position::None);
        }
    }else{
        //si il est dans un portail, on l'ajoute au joueur
        for (const auto& portalPtr : list_portal) {
            Portal& portal = *portalPtr;

            if (portal.rect.on_screen() && player.get_rect_collision().intersects(portal.rect)) {

                if (portal.type == PortalType::LayerChanger || portal.type == PortalType::House) {
                    player.set_interaction_portal(&portal);
                    player.set_portal_entry_direction(player.get_rect_collision().get_position_relative(portal.rect).first);
                    break;
                }

                else if (portal.type == PortalType::MapChanger) {
                    auto* mapChanger = dynamic_cast<MapChanger*>(&portal);
                    if (mapChanger) {
                        new_map_name = mapChanger->value;
                        break;
                    }
                }
            }
        }

    }    
    if(new_map_name != "null"){
        map.init_change_map(new_map_name, player);
    }
    if(player_changed_layer){
        map.combine_collide_rect(player);
    }
    Console::log("Player layer : " + std::to_string(player.get_layer()));
}

void show_debug_info(const Map& map, const SceCtrlData& pad, const SceCtrlData& previousPad) {
    Console::log("Nombre d'arbres : " + std::to_string(list_tree->size()));

    if ((pad.buttons & SCE_CTRL_TRIANGLE) && !(previousPad.buttons & SCE_CTRL_TRIANGLE)) {
        Console::mode = Console::mode == 0 ? 1 : 0;
    }
    for (const auto& [layerIndex, shapes] : list_collide_rect_by_layer) {
        for (const auto& shape : shapes) {
            std::visit([&](const auto& obj) {
                using T = std::decay_t<decltype(obj)>;

                if constexpr (std::is_same_v<T, Rect>) {
                    vita2d_draw_rectangle(
                        obj.get_position_x() * ZOOM - map.posX,
                        obj.get_position_y() * ZOOM - map.posY,
                        obj.get_width() * ZOOM,
                        obj.get_height() * ZOOM,
                        RGBA8(255, 0, 0, 255)
                    );
                } else if constexpr (std::is_same_v<T, Polygon>) {
                    const auto& points = obj.get_points();
                    if (points.size() < 3) return;

                    const int posX = obj.get_position_x();
                    const int posY = obj.get_position_y();

                    for (size_t i = 0; i < points.size(); ++i) {
                        float x1 = (points[i].first + posX) * ZOOM - map.posX;
                        float y1 = (points[i].second + posY) * ZOOM - map.posY;

                        float x2 = (points[(i + 1) % points.size()].first + posX) * ZOOM - map.posX;
                        float y2 = (points[(i + 1) % points.size()].second + posY) * ZOOM - map.posY;

                        vita2d_draw_line(x1, y1, x2, y2, RGBA8(255, 0, 0, 255));
                    }
                }
            }, shape);
        }
    }


    // Afficher les formes globales (en dehors des couches)
    for (const auto& shape : list_collide_rect_global) {
        std::visit([&](const auto& obj) {
            using T = std::decay_t<decltype(obj)>;

            if constexpr (std::is_same_v<T, Rect>) {
                vita2d_draw_rectangle(
                    obj.get_position_x() * ZOOM - map.posX,
                    obj.get_position_y() * ZOOM - map.posY,
                    obj.get_width() * ZOOM,
                    obj.get_height() * ZOOM,
                    RGBA8(255, 0, 0, 255)
                );
            } else if constexpr (std::is_same_v<T, Polygon>) {
                const auto& points = obj.get_points();
                if (points.size() < 3) return;

                const int posX = obj.get_position_x();
                const int posY = obj.get_position_y();

                for (size_t i = 0; i < points.size(); ++i) {
                    float x1 = (points[i].first + posX) * ZOOM - map.posX;
                    float y1 = (points[i].second + posY) * ZOOM - map.posY;

                    float x2 = (points[(i + 1) % points.size()].first + posX) * ZOOM - map.posX;
                    float y2 = (points[(i + 1) % points.size()].second + posY) * ZOOM - map.posY;

                    vita2d_draw_line(x1, y1, x2, y2, RGBA8(255, 0, 0, 255));
                }
            }
        }, shape);
    }
    //afficher tout des portals
    for (const std::unique_ptr<Portal>& portal : list_portal) {
        vita2d_draw_rectangle(
            portal->rect.get_position_x() * ZOOM - map.posX,
            portal->rect.get_position_y() * ZOOM - map.posY,
            portal->rect.get_width() * ZOOM,
            portal->rect.get_height() * ZOOM,
            RGBA8(0, 255, 0, 255)
        );
    }

    
    vita2d_draw_line(SCREEN_WIDTH / 2, 0, SCREEN_WIDTH / 2, SCREEN_HEIGHT, RGBA8(255, 0, 0, 255));
    vita2d_draw_line(0, SCREEN_HEIGHT / 2, SCREEN_WIDTH, SCREEN_HEIGHT / 2, RGBA8(255, 0, 0, 255));
    Console::show();
}