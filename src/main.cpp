#include "include/main.hpp"

int main() {
    SceCtrlData pad;
    int state = IDLE;

    // Variable pour savoir si la touche Select est appuyée ou non
    bool selectPressed = false;

    // Charger la carte
    
    

    // Initialisation Vita2D
    vita2d_init();
    vita2d_set_clear_color(RGBA8(0x40, 0x40, 0x40, 0xFF));
    
    Map map;
    // Créer un joueur au centre de l'écran
    Player player(SCREEN_WIDTH / 2 - PLAYER_SPRITE_WIDTH / 2, SCREEN_HEIGHT / 2 - PLAYER_SPRITE_HEIGHT / 2, 2);

    memset(&pad, 0, sizeof(pad));

    // Boucle principale du jeu
    while (1) {
        // Lecture des contrôles
        sceCtrlPeekBufferPositive(0, &pad, 1);

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

        // Déplacer le joueur
        player.move(move_x, move_y);
        player.update_animation(1.0f / 60.0f);  // Supposons que le jeu tourne à 60 FPS

        // Début du dessin
        vita2d_start_drawing();
        vita2d_clear_screen();

        // Dessiner la carte
        map.draw();

        // Dessiner le joueur
        player.draw();

        // Fin du dessin
        vita2d_end_drawing();
        vita2d_swap_buffers();
    }

    // Libération des ressources
    vita2d_fini();
    sceKernelExitProcess(0);

    return 0;
}
