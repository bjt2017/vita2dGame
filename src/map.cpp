#include "include/map.hpp"

Map::Map() {
    tileset = vita2d_load_PNG_file("app0:assets/assets.png");
    if (!tileset) {
        printf("Erreur lors du chargement des assets\n");
        sceKernelExitProcess(0);
    }
    if (!map.load("app0:assets/map.tmx")) {
        printf("Erreur lors du chargement de la carte\n");
        sceKernelExitProcess(0);
    }
    tileWidth = map.getTileSize().x;
    tileHeight = map.getTileSize().y;

    mapWidth = map.getTileCount().x;
    mapHeight = map.getTileCount().y;

    posX = 0;
    posY = 0;

    // Désactiver l'interpolation bilinéaire pour un rendu net des tiles
    vita2d_texture_set_filters(tileset, SCE_GXM_TEXTURE_FILTER_POINT, SCE_GXM_TEXTURE_FILTER_POINT);
}

Map::~Map() {
    vita2d_free_texture(tileset);
}

void Map::move(int x, int y, Player *player) {
    // Log de la position actuelle de la carte et du joueur
    console.log("Map Position : {" + std::to_string(posX) + " , " + std::to_string(posY) + "}");

    // Calcul des limites du centre de l'écran
    int screenCenterX = SCREEN_WIDTH / 2;
    int screenCenterY = SCREEN_HEIGHT / 2;

    // Déplacer le joueur en fonction de l'entrée utilisateur
    player->move(x, y);

    // Vérification des limites pour recentrer le joueur
    if (player->posx != screenCenterX) {
        if (player->posx > screenCenterX && posX + x <= (mapWidth * tileWidth * ZOOM) - SCREEN_WIDTH) {
            posX += player->posx - screenCenterX; // Ajustement de la carte en fonction du déplacement du joueur
            player->posx = screenCenterX; // Recentrer le joueur
        } else if (player->posx < screenCenterX && posX > 0) {
            posX += player->posx - screenCenterX;
            player->posx = screenCenterX; // Recentrer le joueur
        }
    }

    if (player->posy != screenCenterY) {
        if (player->posy > screenCenterY && posY + y <= (mapHeight * tileHeight * ZOOM) - SCREEN_HEIGHT) {
            posY += player->posy - screenCenterY; // Ajustement de la carte en fonction du déplacement du joueur
            player->posy = screenCenterY; // Recentrer le joueur
        } else if (player->posy < screenCenterY && posY > 0) {
            posY += player->posy - screenCenterY;
            player->posy = screenCenterY; // Recentrer le joueur
        }
    }

    // S'assurer que la carte ne dépasse pas les limites
    if (posX < 0) {
        posX = 0;
    } else if (posX > (mapWidth * tileWidth * ZOOM) - SCREEN_WIDTH) {
        posX = (mapWidth * tileWidth * ZOOM) - SCREEN_WIDTH;
    }

    if (posY < 0) {
        posY = 0;
    } else if (posY > (mapHeight * tileHeight * ZOOM) - SCREEN_HEIGHT) {
        posY = (mapHeight * tileHeight * ZOOM) - SCREEN_HEIGHT;
    }
}







void Map::draw() {
    const auto& layers = this->map.getLayers();

    console.log("Nombre de couches: " + std::to_string(layers.size()));
    console.log("Taille de la carte: {" + std::to_string(mapWidth * tileWidth * ZOOM) + " , " + std::to_string(mapHeight * tileHeight * ZOOM) + "}");

    // Parcourir les couches de la carte
    for (const auto& layer : layers) {
        if (layer->getType() == tmx::Layer::Type::Tile && layer->getName() == "Tile Layer 1") {
            const auto* tileLayer = dynamic_cast<const tmx::TileLayer*>(layer.get());
            if (tileLayer) {
                const auto& tiles = tileLayer->getTiles();
                int width = tileLayer->getSize().x;
                int height = tileLayer->getSize().y;

                // Boucle à travers les tuiles de la couche
                for (int y = 0; y < height; ++y) {
                    for (int x = 0; x < width; ++x) {
                        tmx::TileLayer::Tile tile = tiles[y * width + x];

                        if (tile.ID != 0) { // Si une tuile est présente (ID != 0)
                            // Calcul des positions pour dessiner les tuiles
                            int srcX = (tile.ID - 1) % (vita2d_texture_get_width(tileset) / tileWidth) * tileWidth;
                            int srcY = (tile.ID - 1) / (vita2d_texture_get_width(tileset) / tileWidth) * tileHeight;

                            // Calcul de la position de la tuile sur l'écran, en tenant compte de posX et posY
                            int screenX = (x * tileWidth * ZOOM) - posX;
                            int screenY = (y * tileHeight * ZOOM) - posY;

                            // Ne dessiner que les tuiles visibles à l'écran
                            if (screenX + (tileWidth * ZOOM) >= 0 && screenX < SCREEN_WIDTH &&
                                screenY + (tileHeight * ZOOM) >= 0 && screenY < SCREEN_HEIGHT) {

                                // Dessiner la tuile avec le facteur de zoom
                                vita2d_draw_texture_part_scale(
                                    tileset,
                                    screenX, screenY, // Position sur l'écran
                                    srcX, srcY, // Position dans la texture source
                                    tileWidth, tileHeight, // Taille de la tuile
                                    ZOOM, ZOOM // Facteur de zoom
                                );
                            }
                        }
                    }
                }
            }
        }
    }
}
