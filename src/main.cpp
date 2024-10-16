#include "include/main.hpp"

static void move(SceCtrlData *pad, Player *player, Map *map);

float ZOOM = 3.0f;
Console console;

int main() {
    SceCtrlData pad;
    SceTouchData touch_data;

    // Variable pour savoir si la touche Select est appuyée ou non
    bool selectPressed = false;
    // Initialisation Vita2D
    vita2d_init();
    vita2d_set_clear_color(RGBA8(0x40, 0x40, 0x40, 0xFF));

    Map map;
    // Créer un joueur au centre de l'écran
    Player player(SCREEN_WIDTH / 2 - (PLAYER_SPRITE_WIDTH*ZOOM) / 2, SCREEN_HEIGHT / 2 - (PLAYER_SPRITE_HEIGHT*ZOOM) / 2, 2);

    console.init();

    // Initialisation du touchpad
    sceTouchSetSamplingState(SCE_TOUCH_PORT_FRONT, SCE_TOUCH_SAMPLING_STATE_START);
    sceTouchEnableTouchForce(SCE_TOUCH_PORT_FRONT);

    // Initialisation du pad
    memset(&pad, 0, sizeof(pad));

    // Boucle principale du jeu
    while (1) {
        // Lecture des contrôles
        sceCtrlPeekBufferPositive(0, &pad, 1);

        // Lecture des touches sur l'écran
        sceTouchPeek(SCE_TOUCH_PORT_FRONT, &touch_data, 1);

        // Si l'utilisateur appuie sur Start, on quitte
        if (pad.buttons & SCE_CTRL_START)
            break;

        // Gestion de l'état du bouton Select
        if (pad.buttons & SCE_CTRL_SELECT) {
            if (!selectPressed) {
                player.debug = !player.debug;
                selectPressed = true;  // Indique que le bouton a été appuyé
            }
        } else {
            selectPressed = false;  // Réinitialise l'état quand la touche est relâchée
        }

        
        //pour chaque touch sur l'ecran faire un console.log
        for (SceUInt32 i = 0; i < touch_data.reportNum; i++) {
            if (touch_data.report[i].x != 0 && touch_data.report[i].y != 0) {
                console.log("Touch detected at {X,Y} : {" + std::to_string(touch_data.report[i].x) + " , " + std::to_string(touch_data.report[i].y)+"}");
            }
        }
        //si y a pas de touch sur l'ecran afficher le message
        if (touch_data.reportNum == 0) {
            console.log("No touch detected");
        }

        // Déplacer le joueur
        

        // Début du dessin
        vita2d_start_drawing();
        vita2d_clear_screen();

        move(&pad, &player, &map);

        // Dessiner la carte
        map.draw();

        // Dessiner le joueur
        player.draw();

        if(player.debug){
            console.show();
            
            //afficher 2 lignes horizontales et vericales pour pouvoir voir le centre de l'ecran
            vita2d_draw_line(SCREEN_WIDTH/2, 0, SCREEN_WIDTH/2, SCREEN_HEIGHT, RGBA8(255, 0, 0, 255));
            vita2d_draw_line(0, SCREEN_HEIGHT/2, SCREEN_WIDTH, SCREEN_HEIGHT/2, RGBA8(255, 0, 0, 255));
        }

        vita2d_end_drawing();
        vita2d_swap_buffers();
        console.clear();
    }

    vita2d_fini();
    sceKernelExitProcess(0);

    return 0;
}

static void move(SceCtrlData *pad, Player *player, Map *map) {
    double move_x = 0, move_y = 0;
    int state = IDLE;

    if (pad->buttons & SCE_CTRL_UP) {
        move_y = -PLAYER_SPEED;
        state = WALK;
    } 
    if (pad->buttons & SCE_CTRL_DOWN) {
        move_y = PLAYER_SPEED;
        state = WALK;
    } 
    if (pad->buttons & SCE_CTRL_LEFT) {
        move_x = -PLAYER_SPEED;
        state = WALK;
    }
    if (pad->buttons & SCE_CTRL_RIGHT) {
        move_x = PLAYER_SPEED;
        state = WALK;
    }

    player->state = state;
    if (state == WALK && move_x != 0) {
        player->direction = move_x > 0 ? RIGHT : LEFT;
    }

    console.log("Player speed : " + std::to_string(std::round(PLAYER_SPEED * ZOOM)));
    console.log("Move : {" + std::to_string(std::round(move_x * ZOOM)) + " , " + std::to_string(std::round(move_y * ZOOM)) + "}");

    map->move(std::round(move_x * ZOOM), std::round(move_y * ZOOM), player);

    player->update_animation(1.0f / 60.0f); 
}
