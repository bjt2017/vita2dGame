#include "include/main.hpp"

void move(SceCtrlData *pad, Player *player, Map *map);
void draw(Player *player, Map *map);

float ZOOM = 3.0f;
Console console;

std::vector<tmx::Object> list_collide_objects = {}; 
std::vector<Tree> list_tree = {};  

std::vector<tmx::Object> list_collide_objects_on_screen = {}; 
std::vector<Tree> list_tree_on_screen = {};  

int main() {
    SceCtrlData pad, previousPad;
    SceTouchData touch_data;

    // Initialisation Vita2D
    vita2d_init();
    vita2d_set_clear_color(RGBA8(0x40, 0x40, 0x40, 0xFF));

    Map map;
    // Créer un joueur au centre de l'écran
    Player player(SCREEN_WIDTH / 2 - (PLAYER_SPRITE_WIDTH*ZOOM) / 2, SCREEN_HEIGHT / 2 - (PLAYER_SPRITE_HEIGHT*ZOOM) / 2, 2);

    map.init(&player);

    console.init();

    // Initialisation du touchpad
    sceTouchSetSamplingState(SCE_TOUCH_PORT_FRONT, SCE_TOUCH_SAMPLING_STATE_START);
    sceTouchEnableTouchForce(SCE_TOUCH_PORT_FRONT);

    // Initialisation du pad
    memset(&pad, 0, sizeof(pad));
    memset(&previousPad, 0, sizeof(previousPad));  // Initialiser le pad précédent à 0

    // Boucle principale du jeu
    while (1) {
        // Lecture des contrôles
        sceCtrlPeekBufferPositive(0, &pad, 1);

        // Lecture des touches sur l'écran
        sceTouchPeek(SCE_TOUCH_PORT_FRONT, &touch_data, 1);

        // Si l'utilisateur appuie sur Start, on quitte
        if (pad.buttons & SCE_CTRL_START)
            break;

        // Gestion de l'état du bouton Select (transition de non appuyé à appuyé)
        if ((pad.buttons & SCE_CTRL_SELECT) && !(previousPad.buttons & SCE_CTRL_SELECT)) {
            player.debug = !player.debug;
        } 
        //pour chaque touch sur l'écran faire un console.log
        for (SceUInt32 i = 0; i < touch_data.reportNum; i++) {
            if (touch_data.report[i].x != 0 && touch_data.report[i].y != 0) {
                console.log("Touch detected at {X,Y} : {" + std::to_string(touch_data.report[i].x) + " , " + std::to_string(touch_data.report[i].y) + "}");
            }
        }
        //si y a pas de touch sur l'écran afficher le message
        if (touch_data.reportNum == 0) {
            console.log("No touch detected");
        }

        vita2d_start_drawing();
        vita2d_clear_screen();

        Utils::update_list(&map);

        // Déplacer le joueur
        move(&pad, &player, &map);

        draw(&player, &map);

        if (player.debug) {
            console.log("Nombre d'arbres : " + std::to_string(list_tree.size()));

            // Gestion de l'état du bouton Triangle (transition de non appuyé à appuyé)
            if ((pad.buttons & SCE_CTRL_TRIANGLE) && !(previousPad.buttons & SCE_CTRL_TRIANGLE)) {
                console.mode = console.mode == 0 ? 1 : 0;
            }
            // Afficher tous les objets de collision
            for (const auto& object : list_collide_objects_on_screen) {
                if (object.getShape() == tmx::Object::Shape::Rectangle) {
                    // Dessin d'un rectangle
                    vita2d_draw_rectangle(
                        object.getAABB().left * ZOOM - map.posX,
                        object.getAABB().top * ZOOM - map.posY,
                        object.getAABB().width * ZOOM,
                        object.getAABB().height * ZOOM,
                        RGBA8(255, 0, 0, 255)
                    );
                } else if (object.getShape() == tmx::Object::Shape::Polygon) {
                    const auto& points = object.getPoints();
                    const auto& position = object.getPosition(); // Position globale de l'objet

                    // Vérifier s'il y a assez de points pour former un polygone
                    if (points.size() < 3) continue;

                    // Dessiner les bordures du polygone en reliant chaque point au suivant
                    for (size_t i = 0; i < points.size(); ++i) {
                        // Calculer les coordonnées réelles en tenant compte de la position globale de l'objet
                        float x1 = (position.x + points[i].x) * ZOOM - map.posX;
                        float y1 = (position.y + points[i].y) * ZOOM - map.posY;

                        float x2 = (position.x + points[(i + 1) % points.size()].x) * ZOOM - map.posX;
                        float y2 = (position.y + points[(i + 1) % points.size()].y) * ZOOM - map.posY;

                        // Dessiner la ligne entre deux points consécutifs
                        vita2d_draw_line(
                            x1, y1, // Point actuel
                            x2, y2, // Point suivant (boucle avec le modulo pour revenir au premier point)
                            RGBA8(255, 0, 0, 255)  // Rouge opaque pour les bordures
                        );
                    }
                }
                
            }
            // Afficher 2 lignes horizontales et verticales pour voir le centre de l'écran
            vita2d_draw_line(SCREEN_WIDTH / 2, 0, SCREEN_WIDTH / 2, SCREEN_HEIGHT, RGBA8(255, 0, 0, 255));
            vita2d_draw_line(0, SCREEN_HEIGHT / 2, SCREEN_WIDTH, SCREEN_HEIGHT / 2, RGBA8(255, 0, 0, 255));

            console.show();
        }

        player.update_animation(1.0f / 60.0f);

        vita2d_end_drawing();
        vita2d_swap_buffers();
        console.clear();

        // Mémoriser l'état actuel du pad pour la prochaine boucle
        previousPad = pad;
    }

    vita2d_fini();
    sceKernelExitProcess(0);

    return 0;
}
void draw(Player *player, Map *map)
{
    const auto& layers = map->map.getLayers();

    console.log("Nombre de couches: " + std::to_string(layers.size()));
    console.log("Taille de la carte: {" + std::to_string(map->mapWidth * map->tileWidth * ZOOM) + " , " + std::to_string(map->mapHeight * map->tileHeight * ZOOM) + "}");

    for (const auto& layer : layers) {
        
        if (layer->getType() == tmx::Layer::Type::Tile) {
            const auto* tileLayer = dynamic_cast<const tmx::TileLayer*>(layer.get());

            std::string name = layer->getName();
            int actual_layer = std::stoi(Utils::split(name, '_', 1));
            std::string layer_name = Utils::split(name, '_', 0);

            bool draw_player = actual_layer == player->current_layer && Utils::split(name, '_', 2) == "over" && layer_name =="tiles";
            
            console.log("couche, over_player : " + std::to_string(actual_layer) + " , " + std::to_string(draw_player));

            if (tileLayer) {
                if(draw_player){
                    player->draw();
                }
                map->drawLayer(tileLayer, layer_name == "arbre");
            }
        }
    }
}




void move(SceCtrlData *pad, Player *player, Map *map) {
    // Log de la position actuelle de la carte et du joueur
    console.log("Map Position : {" + std::to_string(map->posX) + " , " + std::to_string(map->posY) + "}");

    int x = 0, y = 0;

    // Si le bouton carré est appuyé, changer l'état en Axe
    if(pad->buttons & SCE_CTRL_SQUARE){
        player->changeState(Axe);
    }
    if(player->current_state.can_move){
        // Gestion du mouvement vertical
        if(pad->buttons & SCE_CTRL_UP){
            y = floor(-PLAYER_SPEED * ZOOM);
            player->direction_y = UP;

        } else if(pad->buttons & SCE_CTRL_DOWN){
            y = ceil(PLAYER_SPEED * ZOOM);
            player->direction_y = DOWN;
        }

        // Gestion du mouvement horizontal
        if(pad->buttons & SCE_CTRL_LEFT){
            x = floor(-PLAYER_SPEED * ZOOM);
            player->direction_x = LEFT;
        } else if(pad->buttons & SCE_CTRL_RIGHT){
            x = ceil(PLAYER_SPEED * ZOOM);
            player->direction_x = RIGHT;
        }
    }
    
    if(x==0 && y==0){
        player->changeState(IDLE);
    }

    console.log("Move : {" + std::to_string(x) + " , " + std::to_string(y) + "}");

    // Vérification des limites de déplacement de la carte
    bool map_can_move_left = map->posX + x <= (map->mapWidth * map->tileWidth * ZOOM) - SCREEN_WIDTH;
    bool map_can_move_right = map->posX + x >= 0;

    bool map_can_move_up = map->posY + y <= (map->mapHeight * map->tileHeight * ZOOM) - SCREEN_HEIGHT;
    bool map_can_move_down = map->posY + y >= 0;

    int maxX = x;
    int maxY = y;

    for (const auto& object : list_collide_objects_on_screen) {
        int collideX;
        int collideY;
        if(object.getShape() == tmx::Object::Shape::Rectangle){
            auto collisionResult = player->collision_rect(object, map, x, y);
            collideX = collisionResult.first;
            collideY = collisionResult.second;
            
        } else if (object.getShape() == tmx::Object::Shape::Polygon){
            auto collisionResult = player->collision_polygon(object, map, x, y);
            collideX = collisionResult.first;
            collideY = collisionResult.second;
        }
        maxX = (x > 0) ? std::min(maxX, collideX) : std::max(maxX, collideX);
        maxY = (y > 0) ? std::min(maxY, collideY) : std::max(maxY, collideY);
    }

    for (Tree& tree : list_tree){
        const bool colision = player->collision_bounding_box({tree.x*ZOOM,tree.y*ZOOM,tree.width*ZOOM,tree.heigth*ZOOM},map);
        console.log("colision : " + std::to_string(colision));
        if(colision){
            tree.status = SELECT;
        } else{
            tree.status = NORMAL;
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


    // Vérification si le joueur peut se déplacer dans les directions spécifiées
    if(map_can_move_left && player->direction_x == RIGHT && player->posx_player == player_center_x){
        map->move(maxX, 0);
    } else if(player->direction_x == RIGHT){
        if(player->posx_player < player_center_x && player->posx_player + maxX > player_center_x){
            maxX = player_center_x - player->posx_player;
        }
        player->move(maxX, 0);
    }
    
    if(map_can_move_right && player->direction_x == LEFT && player->posx_player == player_center_x){
        map->move(maxX, 0);
    } else if(player->direction_x == LEFT){
        if(player->posx_player > player_center_x && player->posx_player + maxX < player_center_x){
            maxX = player_center_x - player->posx_player;
        }
        player->move(maxX, 0);
    }

    if(map_can_move_up && player->direction_y == DOWN && player->posy_player == player_center_y){
        map->move(0, maxY);
    } else if(player->direction_y == DOWN){
        if(player->posy_player < player_center_y && player->posy_player + maxY > player_center_y){
            maxY = player_center_y - player->posy_player;
        }
        player->move(0, maxY);
    }

    if(map_can_move_down && player->direction_y == UP && player->posy_player == player_center_y){
        map->move(0, maxY);
    }
    else if(player->direction_y == UP){
        if(player->posy_player > player_center_y && player->posy_player + maxY < player_center_y){
            maxY = player_center_y - player->posy_player;
        }
        player->move(0, maxY);
    }

    // Changer l'état du joueur selon qu'il bouge ou non
    player->changeState((maxX == 0 && maxY == 0) ? IDLE : WALK);
}
