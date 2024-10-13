#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <cstddef>

#include <tmxlite/Map.hpp>
#include <tmxlite/TileLayer.hpp>
#include <psp2/appmgr.h>

#include <psp2/ctrl.h>
#include <psp2/kernel/processmgr.h>

#include <vita2d.h>

#include <cstddef> 


extern unsigned char _binary_assets_assets_png_start;
extern unsigned char _binary_assets_base_walk_strip8_png_start;
extern unsigned char _binary_assets_base_idle_strip9_png_start;
extern unsigned char _binary_assets_bowlhair_idle_strip9_png_start;



#define IDLE 0
#define WALK 1

#define LEFT 0
#define RIGHT 1

#define TILE_SIZE 16  // Taille d'une tuile
#define SCREEN_WIDTH 960  // Largeur de l'écran
#define SCREEN_HEIGHT 544  // Hauteur de l'écran


#define PLAYER_SPRITE_X 40  // Position verticale du sprite de joueur dans l'image
#define PLAYER_SPRITE_Y 21  // Position horizontale du sprite de joueur dans l'image
#define PLAYER_SPRITE_WIDTH 16  // Largeur du sprite de joueur
#define PLAYER_SPRITE_HEIGHT 19  // Hauteur du sprite de joueur
#define PLAYER_SPRITE_SPACING 80  // Espace entre chaque sprite (horizontalement)

#define COLLIDE_ZONE_Y 13


#define ZOOM 2.0f

class Player {
public:
    int posx;
    int posy;
    int speed;
    int frame;
    float frame_duration;
    float animation_timer;
    int direction;
    int state;

    bool debug;


    vita2d_texture *texture_idle;
    vita2d_texture *texture_walk;
    vita2d_texture *texture_bowlhair_idle;
    vita2d_pgf *font;

    
    
    Player(int x, int y, int speed_value) {
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

    ~Player() {
        vita2d_free_texture(texture_walk);
        vita2d_free_texture(texture_idle);
        vita2d_free_texture(texture_bowlhair_idle);
        vita2d_free_pgf(font);
    }

    void move(int x, int y) {
        // Met à jour la position
        posx += x * speed;
        posy += y * speed;

        // Met à jour la direction
        if (x < 0) {
            direction = LEFT; // Va vers la gauche
        } else if (x > 0) {
            direction = RIGHT; // Va vers la droite
        }

        // Si le joueur bouge, mettre à jour l'état à WALK, sinon IDLE
        state = (x != 0 || y != 0) ? WALK : IDLE;
    }

    void update_animation(float delta_time) {
        animation_timer += delta_time;
        if (animation_timer >= frame_duration) {
            frame = (frame + 1) % 8;  // 8 frames pour l'animation
            animation_timer = 0.0f;
        }
    }

    void draw() {
        int src_x = PLAYER_SPRITE_X + frame * (PLAYER_SPRITE_SPACING + PLAYER_SPRITE_WIDTH);
        float flip = (direction == LEFT) ? -ZOOM : ZOOM;  // Inverser l'échelle pour le mouvement à gauche

        // Ajuster la position pour éviter que le joueur ne se déplace lorsqu'on inverse l'image
        float draw_x = (direction == LEFT) ? posx + (PLAYER_SPRITE_WIDTH * ZOOM) + 1 : posx;

        // Afficher le texte du state (idle ou walk)
        

        // Dessiner le corps en fonction de l'état (marche ou idle) et de la direction
        if (state == WALK) {
            vita2d_draw_texture_part_scale(texture_walk, draw_x, posy, 
                                           src_x, PLAYER_SPRITE_Y, PLAYER_SPRITE_WIDTH, PLAYER_SPRITE_HEIGHT, 
                                           flip, ZOOM);
        } else {
            vita2d_draw_texture_part_scale(texture_idle, draw_x, posy, 
                                           src_x, PLAYER_SPRITE_Y, PLAYER_SPRITE_WIDTH, PLAYER_SPRITE_HEIGHT, 
                                           flip, ZOOM);
        }

        // Dessiner les cheveux du joueur
        vita2d_draw_texture_part_scale(texture_bowlhair_idle, draw_x, posy, 
                                       src_x, PLAYER_SPRITE_Y, PLAYER_SPRITE_WIDTH, PLAYER_SPRITE_HEIGHT, 
                                       flip, ZOOM);


        if (debug) {
            vita2d_pgf_draw_textf(font, 10, 30, RGBA8(255, 255, 255, 255), 1.0f, "State: %s", (state == WALK) ? "WALK" : "IDLE");

            // Ajouter des lignes rouges autour du joueur pour les repères
            vita2d_draw_line(posx, posy, posx + PLAYER_SPRITE_WIDTH * ZOOM, posy, RGBA8(255, 0, 0, 255));
            vita2d_draw_line(posx, posy, posx, posy + PLAYER_SPRITE_HEIGHT * ZOOM, RGBA8(255, 0, 0, 255)); 
            vita2d_draw_line(posx + PLAYER_SPRITE_WIDTH * ZOOM, posy, posx + PLAYER_SPRITE_WIDTH * ZOOM, posy + PLAYER_SPRITE_HEIGHT * ZOOM, RGBA8(255, 0, 0, 255)); // Ligne de droite
            vita2d_draw_line(posx, posy + PLAYER_SPRITE_HEIGHT * ZOOM, posx + PLAYER_SPRITE_WIDTH * ZOOM, posy + PLAYER_SPRITE_HEIGHT * ZOOM, RGBA8(255, 0, 0, 255)); // Ligne du bas

            // dessiner la zone de collision
            vita2d_draw_rectangle(posx, posy + COLLIDE_ZONE_Y, PLAYER_SPRITE_WIDTH * ZOOM, PLAYER_SPRITE_HEIGHT * ZOOM - COLLIDE_ZONE_Y, RGBA8(255, 0, 0, 255));
        }
    }
};



void drawMap(const tmx::Map& map, vita2d_texture* tileset, int tileWidth, int tileHeight)
{
    const auto& layers = map.getLayers();

    // Parcourir les couches de la carte
    for (const auto& layer : layers)
    {
        if (layer->getType() == tmx::Layer::Type::Tile && layer->getName() == "Tile Layer 1")
        {
            const auto* tileLayer = dynamic_cast<const tmx::TileLayer*>(layer.get());
            if (tileLayer)
            {
                const auto& tiles = tileLayer->getTiles();
                int width = tileLayer->getSize().x;
                int height = tileLayer->getSize().y;

                // Boucle à travers les tuiles de la couche
                for (int y = 0; y < height; ++y)
                {
                    for (int x = 0; x < width; ++x)
                    {
                        tmx::TileLayer::Tile tile = tiles[y * width + x];

                        if (tile.ID != 0) // Si une tuile est présente (ID != 0)
                        {
                            // Calcul des positions pour dessiner les tuiles
                            int srcX = (tile.ID - 1) % (vita2d_texture_get_width(tileset) / tileWidth) * tileWidth;
                            int srcY = (tile.ID - 1) / (vita2d_texture_get_width(tileset) / tileWidth) * tileHeight;

                            // Dessiner la tuile à la position correcte
                            vita2d_draw_texture_part(tileset,
                                                     x * tileWidth, y * tileHeight, // Position sur l'écran
                                                     srcX, srcY,                    // Position dans la texture source
                                                     tileWidth, tileHeight);        // Taille de la tuile
                        }
                    }
                }
            }
        }
    }
}


int main() {
    SceCtrlData pad;
    vita2d_texture *tileset;
    int state = IDLE;

    // Variable pour savoir si la touche Select est appuyée ou non
    bool selectPressed = false;

    // Charger la carte
    tmx::Map map;
    if (!map.load("app0:assets/map.tmx"))
    {
        printf("Erreur lors du chargement de la carte\n");
        return -1;
    }

    int tileWidth = map.getTileSize().x;
    int tileHeight = map.getTileSize().y;

    // Initialisation Vita2D
    vita2d_init();
    vita2d_set_clear_color(RGBA8(0x40, 0x40, 0x40, 0xFF));

    // Charger les textures
    tileset = vita2d_load_PNG_buffer(&_binary_assets_assets_png_start);
    if (!tileset) {
        printf("Erreur lors du chargement de l'image source\n");
        return -1;
    }

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
        drawMap(map, tileset, tileWidth, tileHeight);

        // Dessiner le joueur
        player.draw();

        // Fin du dessin
        vita2d_end_drawing();
        vita2d_swap_buffers();
    }

    // Libération des ressources
    vita2d_fini();
    vita2d_free_texture(tileset);
    sceKernelExitProcess(0);

    return 0;
}
