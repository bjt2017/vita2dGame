#include "include/map.hpp"

std::unordered_set<int> tree_ids = {2626, 2627, 2690, 2691, 2498, 2499, 2562, 2563,2690,2691};

float Map::water_timer = 0.0f;
int Map::water_frame = 0;

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

    firstTileX = std::max(0, static_cast<int>(posX / (tileWidth * ZOOM)));
    firstTileY = std::max(0, static_cast<int>(posY / (tileHeight * ZOOM)));
    lastTileX = std::min(mapWidth - 1, static_cast<int>((posX + SCREEN_WIDTH) / (tileWidth * ZOOM)));
    lastTileY = std::min(mapHeight - 1, static_cast<int>((posY + SCREEN_HEIGHT) / (tileHeight * ZOOM)));

    vita2d_texture_set_filters(tileset, SCE_GXM_TEXTURE_FILTER_POINT, SCE_GXM_TEXTURE_FILTER_POINT);
}

void Map::init(Player &player){

    // Initialisation de la carte
    
    Rect::set_map_pos(posX, posY, ZOOM);
    // Récupérer les objets de la map sur la couche player->current_layer
    const auto& layers = map.getLayers();
    for (const auto& layer : layers) {
        if (layer->getType() == tmx::Layer::Type::Object) {
            const auto* objectLayer = dynamic_cast<const tmx::ObjectGroup*>(layer.get());

            if (objectLayer) {
                const auto& objects = objectLayer->getObjects();
                std::string name = layer->getName();
                std::string type = Utils::split(name, '_', 0);
                
                std::string layerStr = Utils::split(name, '_', 1);
                if(layerStr.empty()) {
                    Console::warning("Layer name is empty");
                }
                int actual_layer = layerStr.empty() ? 1 : std::stoi(layerStr);
                
                if (actual_layer == player.get_layer() && type == "colision") {
                    for(const auto& object : objects){
                        list_collide_rect.push_back(Rect(object));
                    }
                } else if (type == "animation") {
                    for (const auto& object : objects) {
                        std::string objectName = object.getName();
                  
                        if (objectName == "arbre") {
                            int x = static_cast<int>(object.getPosition().x);
                            int y = static_cast<int>(object.getPosition().y);
                            list_tree.push_back(Tree(x, y,actual_layer)); 
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

    Rect::set_map_pos(posX, posY, ZOOM);

    firstTileX = std::max(0, static_cast<int>(posX / (tileWidth * ZOOM)));
    firstTileY = std::max(0, static_cast<int>(posY / (tileHeight * ZOOM)));
    lastTileX = std::min(mapWidth - 1, static_cast<int>((posX + SCREEN_WIDTH) / (tileWidth * ZOOM)));
    lastTileY = std::min(mapHeight - 1, static_cast<int>((posY + SCREEN_HEIGHT) / (tileHeight * ZOOM)));
}

void Map::update() {
    Map::water_timer += 0.1f;
    if (Map::water_timer >= 1.0f) {
        Map::water_timer = 0;
        Map::water_frame = (water_frame + 1) % 5; 
    }
}


void Map::draw_layer(const tmx::TileLayer& layer, LayerType type){
    const auto& tiles = layer.getTiles();
    for (int y = firstTileY; y <= lastTileY; ++y) {
        for (int x = firstTileX; x <= lastTileX; ++x) {
            tmx::TileLayer::Tile tile = tiles[y * mapWidth + x];
            draw_tile(tile, x, y, type);
        }
    }
}


void Map::draw_tile(tmx::TileLayer::Tile tile, int x, int y, LayerType type) {
    if (tile.ID != 0) {
        unsigned int flip_flags = tile.flipFlags;
        unsigned int new_id = tile.ID;

        switch (type) {
            case LayerType::TREE:
                if (tree_ids.find(new_id) != tree_ids.end()) {
                    Tree* tree = Utils::find_tree(x * 16, y * 16);
                    if (tree != nullptr) {
                        new_id = tree->draw_tile(new_id);
                    }
                }
                break;
            case LayerType::WATER:
                new_id =  new_id + 4*water_frame;
                break;
            default:
                break;
        }

        int srcX = (new_id - 1) % (vita2d_texture_get_width(tileset) / tileWidth) * tileWidth;
        int srcY = (new_id - 1) / (vita2d_texture_get_width(tileset) / tileWidth) * tileHeight;

        int screenX = (x * tileWidth * ZOOM) - posX;
        int screenY = (y * tileHeight * ZOOM) - posY;

        if (screenX + (tileWidth * ZOOM) >= 0 && screenX < SCREEN_WIDTH &&
            screenY + (tileHeight * ZOOM) >= 0 && screenY < SCREEN_HEIGHT) {

            bool flipHorizontal = flip_flags & tmx::TileLayer::FlipFlag::Horizontal;
            bool flipVertical = flip_flags & tmx::TileLayer::FlipFlag::Vertical;
            //bool flipDiagonal = flip_flags & tmx::TileLayer::FlipFlag::Diagonal;

            float scaleX = flipHorizontal ? -ZOOM : ZOOM;
            float scaleY = flipVertical ? -ZOOM : ZOOM;

            int drawX = screenX;
            int drawY = screenY;

            if (flipHorizontal) drawX += tileWidth * ZOOM;
            if (flipVertical) drawY += tileHeight * ZOOM;

            vita2d_draw_texture_part_scale(
                tileset,
                drawX, drawY,
                srcX, srcY,
                tileWidth, tileHeight,
                scaleX, scaleY
            );

        }
    }
}