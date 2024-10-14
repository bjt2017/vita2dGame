#include "main.hpp"
#include "player.hpp"

// Fonction pour dessiner la carte
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

// Point d'entrée principal
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
