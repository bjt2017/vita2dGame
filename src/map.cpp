#include "include/map.hpp"

Map::Map(){
    tileset = vita2d_load_PNG_file("app0:assets/assets.png");
    if(!tileset){
        printf("Erreur lors du chargement de la carte\n");
        sceKernelExitProcess(0);
    }
    if (!map.load("app0:assets/map.tmx"))
    {
        printf("Erreur lors du chargement de la carte\n");
        sceKernelExitProcess(0);
    }
    tileWidth = map.getTileSize().x;
    tileHeight = map.getTileSize().y;
}

Map::~Map(){
    vita2d_free_texture(tileset);
}

void Map::draw()
{
    
    const auto& layers = this->map.getLayers();

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