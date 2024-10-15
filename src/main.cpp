#include "include/main.hpp"


float ZOOM = 3.0f;

int main() {
    SceCtrlData pad;
    SceTouchData touch_data;
    int state = IDLE;

    // Variable pour savoir si la touche Select est appuyée ou non
    bool selectPressed = false;

    // Initialisation Vita2D
    vita2d_init();
    vita2d_set_clear_color(RGBA8(0x40, 0x40, 0x40, 0xFF));

    Console console;

    Map map;
    // Créer un joueur au centre de l'écran
    Player player(SCREEN_WIDTH / 2 - PLAYER_SPRITE_WIDTH / 2, SCREEN_HEIGHT / 2 - PLAYER_SPRITE_HEIGHT / 2, 2);

    // Initialisation du touchpad
    sceTouchSetSamplingState(SCE_TOUCH_PORT_FRONT, SCE_TOUCH_SAMPLING_STATE_START);
    sceTouchEnableTouchForce(SCE_TOUCH_PORT_FRONT);

    //bool zooming = false;
    //float initialDistance = 0.0f;

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

        int move_x = 0, move_y = 0;
        state = IDLE;
        if (pad.buttons & SCE_CTRL_UP) {
            move_y = -1;
            state = WALK;
        } 
        if (pad.buttons & SCE_CTRL_DOWN) {
            move_y = 1;
            state = WALK;
        } 
        if (pad.buttons & SCE_CTRL_LEFT) {
            move_x = -1;
            state = WALK;
        }
        if (pad.buttons & SCE_CTRL_RIGHT) {
            move_x = 1;
            state = WALK;
        } 
        player.state = state;

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

        // zoom avec le touchpad
        /*if (touch_data.reportNum == 2) {
            int x1 = touch_data.report[0].x;
            int y1 = touch_data.report[0].y;
            int x2 = touch_data.report[1].x;
            int y2 = touch_data.report[1].y;

            float currentDistance = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));

            if (!zooming) {
                // Si c'est la première fois qu'on détecte deux doigts, on initialise la distance
                initialDistance = currentDistance;
                zooming = true;
            } else {
                // Calcul du facteur de zoom basé sur la variation de distance
                float zoomFactor = currentDistance / initialDistance;

                ZOOM *= zoomFactor;

                // Limiter le zoom à des valeurs raisonnables
                if (ZOOM < 0.5f) ZOOM = 0.5f;  // Zoom minimal
                if (ZOOM > 4.0f) ZOOM = 4.0f;  // Zoom maximal

                // Réinitialiser la distance initiale pour éviter un zoom trop rapide
                initialDistance = currentDistance;
            }
        } else {
        // Si moins de 2 doigts, désactiver le mode zoom
            zooming = false;
        }*/

        // Déplacer le joueur
        player.move(move_x*ZOOM, move_y*ZOOM);
        player.update_animation(1.0f / 60.0f); 

        // Début du dessin
        vita2d_start_drawing();
        vita2d_clear_screen();

        // Dessiner la carte
        map.draw();

        // Dessiner le joueur
        player.draw();


        console.log("Player position {X,Y} : {" + std::to_string(player.posx) + " , " + std::to_string(player.posy)+"}");
        

        //console.log("Zoom : " + std::to_string(ZOOM));

        if (player.state == WALK) {
            console.log("State : WALK");
        } else {
            console.log("State : IDLE");
        }


        if(player.debug){
            console.show();
        }

    
        
        vita2d_end_drawing();
        vita2d_swap_buffers();
        console.clear();

    }

    vita2d_fini();
    sceKernelExitProcess(0);

    return 0;
}
