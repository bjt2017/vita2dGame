#include "include/map.hpp"

std::unordered_set<int> tree_ids = {2626, 2627, 2690, 2691, 2498, 2499, 2562, 2563};

Map::Map() {
    tileset = vita2d_load_PNG_buffer(&_binary_assets_assets_png_start);
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

    vita2d_texture_set_filters(tileset, SCE_GXM_TEXTURE_FILTER_POINT, SCE_GXM_TEXTURE_FILTER_POINT);
}

void Map::init(Player *player){
    // Récupérer les objets de la map sur la couche player->current_layer
    const auto& layers = map.getLayers();
    for (const auto& layer : layers) {
        if (layer->getType() == tmx::Layer::Type::Object) {
            const auto* objectLayer = dynamic_cast<const tmx::ObjectGroup*>(layer.get());

            if (objectLayer) {
                const auto& objects = objectLayer->getObjects();
                std::string name = layer->getName();
                std::string type = Utils::split(name, '_', 0);
                int actual_layer = std::stoi(Utils::split(name, '_', 1));


                if (actual_layer == player->current_layer && type == "colision") {
                    list_collide_objects = objects;
                } else if (type == "animation") {

                    for (const auto& object : objects) {
                        std::string objectName = object.getName();
                        
                        if (objectName == "arbre") {
                            int x = static_cast<int>(object.getPosition().x);
                            int y = static_cast<int>(object.getPosition().y);
                            Tree arbre(x, y,actual_layer);
                            list_tree.push_back(arbre); 
                        }
                    }
                }
            }
        }
    }
}

Map::~Map() {
    vita2d_free_texture(tileset);
}

void Map::move(int x, int y) {
    posX += x;
    posY += y;
}




void Map::drawLayer(const tmx::TileLayer* layer, bool check){
    const auto& tiles = layer->getTiles();
    int width = layer->getSize().x;
    int height = layer->getSize().y;

    // Boucle à travers les tuiles de la couche
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            tmx::TileLayer::Tile tile = tiles[y * width + x];
            drawTile(tile,x,y,check);
        }
    }
}


void Map::drawTile(tmx::TileLayer::Tile tile, int x, int y, bool check){
    if (tile.ID != 0) { 
        int new_id = tile.ID;
        if(check){
            if (tree_ids.find(new_id) != tree_ids.end()){           
                Tree* tree = Utils::find_tree(x*16,y*16);
                if (tree != nullptr) {
                    new_id = tree->draw_tile(new_id);
                }
            }
        }
        


        int srcX = (new_id - 1) % (vita2d_texture_get_width(tileset) / tileWidth) * tileWidth;
        int srcY = (new_id - 1) / (vita2d_texture_get_width(tileset) / tileWidth) * tileHeight;

        int screenX = (x * tileWidth * ZOOM) - posX;
        int screenY = (y * tileHeight * ZOOM) - posY;

        if (screenX + (tileWidth * ZOOM) >= 0 && screenX < SCREEN_WIDTH &&
            screenY + (tileHeight * ZOOM) >= 0 && screenY < SCREEN_HEIGHT) {

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

void Map::draw() {
    const auto& layers = this->map.getLayers();

    console.log("Nombre de couches: " + std::to_string(layers.size()));
    console.log("Taille de la carte: {" + std::to_string(mapWidth * tileWidth * ZOOM) + " , " + std::to_string(mapHeight * tileHeight * ZOOM) + "}");

    for (const auto& layer : layers) {
        if (layer->getType() == tmx::Layer::Type::Tile) {
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
