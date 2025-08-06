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
    change_map("main", player);
    Tree::load_assets();
    // Initialisation de la carte
    Rect::set_map_pos(posX, posY, ZOOM);
}

void Map::change_map(const std::string& map_name, Player& player) {
    try {
        current_map = &this->get_tile_group({map_name}, true);
        current_map_name = map_name;
    } catch (const std::exception& e) {
        Console::warning("Map not found: " + map_name + " (" + e.what() + ")");
        return;
    }

    

    // Reset des listes
    list_collide_rect.clear();
    list_collide_rect_global.clear();
    list_collide_rect_by_layer.clear();
    list_portal.clear();

    

    // Chargement des couches de collision
    try {
        const auto& collision_group = get_tile_group({"colision"}, false);
        for (const auto& layerPtr : collision_group.getLayers()) {
            if (layerPtr->getType() == tmx::Layer::Type::Object) {
                const auto* objectLayer = dynamic_cast<const tmx::ObjectGroup*>(layerPtr.get());
                if (objectLayer) {
                    const auto& objects = objectLayer->getObjects();
                    std::string name = layerPtr->getName();
                    std::string type = Utils::split(name, '_', 0);
                    std::string layerStr = Utils::split(name, '_', 1);
                    int actual_layer = layerStr.empty() ? 1 : std::stoi(layerStr);

                    for (const auto& object : objects) {
                        if (object.getShape() == tmx::Object::Shape::Rectangle) {
                            (name == "global" ? list_collide_rect_global : list_collide_rect_by_layer[actual_layer]).push_back(Rect(object));
                        } else if (object.getShape() == tmx::Object::Shape::Polygon) {
                            (name == "global" ? list_collide_rect_global : list_collide_rect_by_layer[actual_layer]).push_back(Polygon(object));
                        }
                    }
                }
            }
        }
    } catch (const std::exception& e) {
        Console::warning("Erreur lors du chargement des collisions: " + std::string(e.what()));
    }

    combine_collide_rect(player);

    // Chargement des arbres
    if (list_tree_by_map.find(map_name) == list_tree_by_map.end()) {
        std::vector<Tree> tree_list;

        for (const auto& layerPtr : current_map->getLayers()) {
            if (layerPtr->getType() != tmx::Layer::Type::Object) continue;

            const tmx::ObjectGroup& objectLayer = layerPtr->getLayerAs<tmx::ObjectGroup>();

            if (layerPtr->getName() == "animation") {
                for (const auto& object : objectLayer.getObjects()) {
                    if (object.getName() == "arbre") {
                        int x = static_cast<int>(object.getPosition().x);
                        int y = static_cast<int>(object.getPosition().y);
                        int layerIdx = 1;

                        for (const auto& prop : object.getProperties()) {
                            if (prop.getName() == "layer" && prop.getType() == tmx::Property::Type::Int) {
                                layerIdx = prop.getIntValue();
                                break;
                            }
                        }

                        tree_list.emplace_back(x, y, layerIdx);
                        tree_list.emplace_back(x, y, layerIdx);
                    }
                }
                break;
            }
        }

        // Crée la liste uniquement si elle n'existait pas déjà
        list_tree_by_map.emplace(map_name, std::move(tree_list));
    }

    // Met à jour la référence
    list_tree = &list_tree_by_map.at(map_name);

    // Chargement des portails
    try {
        const auto& portail_group = get_tile_group({"portail"}, false);
        for (const auto& layerPtr : portail_group.getLayers()) {
            if (layerPtr->getType() == tmx::Layer::Type::Object) {
                const auto* objectLayer = dynamic_cast<const tmx::ObjectGroup*>(layerPtr.get());
                if (objectLayer) {
                    const auto& objects = objectLayer->getObjects();
                    std::string name = layerPtr->getName();

                    for (const auto& object : objects) {
                        if (object.getShape() != tmx::Object::Shape::Rectangle) {
                            Console::warning("Unsupported portal shape: " + std::to_string(static_cast<int>(object.getShape())));
                            continue;
                        }

                        if (name == "layer_changer") {
                            list_portal.push_back(std::make_unique<LayerChanger>(object));
                        } 
                        else if (name == "map_changer") {
                            std::string value = "";
                            for (const auto& prop : object.getProperties()) {
                                if (prop.getName() == "map" && prop.getType() == tmx::Property::Type::String) {
                                    value = prop.getStringValue();
                                    break;
                                }
                            }
                            list_portal.push_back(std::make_unique<MapChanger>(object, value));
                        } else if( name == "house"){
                            create_house(object);
                        }
                        else {
                            Console::warning("Unknown portal type: " + name);
                        }
                    }

                }
            }
        }
    } catch (const std::exception& e) {
        Console::warning("Erreur lors du chargement des portails: " + std::string(e.what()));
    }
}

void Map::combine_collide_rect(const Player& player) {
    // Vider la liste des collisions
    list_collide_rect.clear();

    int playerLayer = player.get_layer();

    // Vérifie si la couche du joueur existe dans list_collide_rect_by_layer
    auto it = list_collide_rect_by_layer.find(playerLayer);
    if (it != list_collide_rect_by_layer.end()) {
        const auto& layerShapes = it->second;

        // Réserve l'espace pour éviter des reallocations
        list_collide_rect.reserve(layerShapes.size() + list_collide_rect_global.size());

        // Ajouter les pointeurs des formes de la couche
        for (const auto& shape : layerShapes) {
            list_collide_rect.push_back(const_cast<Shape*>(&shape));
        }

        // Ajouter les pointeurs des formes globales
        for (const auto& shape : list_collide_rect_global) {
            list_collide_rect.push_back(const_cast<Shape*>(&shape));
        }
    } else {
        // Si la couche n'existe pas, on ajoute uniquement les formes globales
        list_collide_rect.reserve(list_collide_rect_global.size());
        for (const auto& shape : list_collide_rect_global) {
            list_collide_rect.push_back(const_cast<Shape*>(&shape));
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

const tmx::LayerGroup& Map::get_tile_group(
    const std::vector<std::string>& path,
    const tmx::LayerGroup& currentGroup,
    size_t idx
) const {
    if (idx >= path.size()) {
        throw std::runtime_error("get_tile_group: path index out of range");
    }

    const std::string& target = path[idx];
    for (const auto& layerPtr : currentGroup.getLayers()) {
        if (layerPtr->getType() == tmx::Layer::Type::Group &&
            layerPtr->getName() == target) {
            const auto& subgroup = layerPtr->getLayerAs<tmx::LayerGroup>();
            if (idx + 1 == path.size()) return subgroup;
            return get_tile_group(path, subgroup, idx + 1);
        }
    }

    throw std::runtime_error("get_tile_group: group not found: " + target);
}

const tmx::LayerGroup& Map::get_tile_group(const std::vector<std::string>& path, bool fromRoot) const {
    if (path.empty()) {
        throw std::runtime_error("get_tile_group: empty path");
    }

    if (fromRoot) {
        for (const auto& layerPtr : map.getLayers()) {
            if (layerPtr->getType() == tmx::Layer::Type::Group &&
                layerPtr->getName() == path[0]) {
                const auto& subgroup = layerPtr->getLayerAs<tmx::LayerGroup>();
                if (path.size() == 1) return subgroup;
                return get_tile_group(path, subgroup, 1);
            }
        }
    } else {
        if (!current_map) {
            throw std::runtime_error("get_tile_group: current_map is null");
        }
        return get_tile_group(path, *current_map, 0);
    }

    throw std::runtime_error("get_tile_group: group not found at root: " + path[0]);
}

tmx::TileLayer& Map::get_tile_layer(const std::string& layerName, const std::vector<std::string>& path) const {
    if (path.empty()) {
        for (const auto& layerPtr : current_map->getLayers()) {
            if (layerPtr->getType() == tmx::Layer::Type::Tile &&
                layerPtr->getName() == layerName) {
                return const_cast<tmx::TileLayer&>(layerPtr->getLayerAs<tmx::TileLayer>());
            }
        }
    } else {
        const auto& group = get_tile_group(path, false);
        for (const auto& layerPtr : group.getLayers()) {
            if (layerPtr->getType() == tmx::Layer::Type::Tile &&
                layerPtr->getName() == layerName) {
                return const_cast<tmx::TileLayer&>(layerPtr->getLayerAs<tmx::TileLayer>());
            }
        }
    }

    throw std::runtime_error("get_tile_layer: tile layer not found: " + layerName);
}

    std::unordered_set<int> Map::get_tiles_under_player(const Player& player, const std::string& layerName, const std::vector<std::string>& path) const {
    std::unordered_set<int> tile_ids;

    try {
        Rect collisionRect = player.get_rect_collision();
        auto [leftPx, rightPx, topPx, bottomPx] = collisionRect.get_position();

        // ajouter le déplacement de la map
        leftPx += posX + 1;
        rightPx += posX - 1;
        topPx += posY + 1;
        bottomPx += posY - 1;

        Console::log("Collision position : " +
                        std::to_string(leftPx) + " " +
                        std::to_string(rightPx) + " " +
                        std::to_string(topPx) + " " +
                        std::to_string(bottomPx));

        const tmx::TileLayer& tileLayer = get_tile_layer(layerName, path);

        const auto& tiles = tileLayer.getTiles();
        if (tiles.empty()) {
            Console::warning("Tile layer is empty: " + layerName);
            return tile_ids;
        }

        int top = topPx / (tileHeight * ZOOM);
        int bottom = bottomPx / (tileHeight * ZOOM);
        int left = leftPx / (tileWidth * ZOOM);
        int right = rightPx / (tileWidth * ZOOM);

        for (int y = top; y <= bottom; ++y) {
            for (int x = left; x <= right; ++x) {
                if (x < 0 || x >= mapWidth || y < 0 || y >= mapHeight) {
                    continue;
                }
                tmx::TileLayer::Tile tile = tiles[y * mapWidth + x];
                tile_ids.insert(tile.ID);
            }
        }
    } catch (const std::exception& e) {
        Console::warning("Exception in get_tiles_under_player: " + std::string(e.what()));
    } catch (...) {
        Console::warning("Unknown exception in get_tiles_under_player");
    }

    return tile_ids;
}

bool Map::player_on_water(const Player& player) const {
    int waterLayerIndex = player.get_layer();
    std::string waterLayerName = "water_" + std::to_string(waterLayerIndex);

    std::unordered_set<int> ids = get_tiles_under_player(player, waterLayerName, {"water"});
    if (ids.empty() || ids.find(0) != ids.end()) {
        return false;
    }
    return true;
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

// std::vector<std::pair<int,int>> Map::get_adjacent_tiles(tmx::TileLayer layer, std::pair<int,int> pos){
//     const auto& tiles = layer.getTiles();
//     std::vector<std::pair<int,int>> result;
//     //droite
//     std::vector<std::pair<int,int>> to_check = {
//         {-1,0},
//         {1,0},
//         {0,1},
//         {1,0}
//     };

//     for(std::pair<int,int> pos_to_check : to_check){
//         if(tiles[(pos_to_check.first + pos.first) * mapWidth + pos_to_check.second+pos.second].ID != 0){
//             result.emplace(get_adjacent_tiles())
//         }
//     }
// }

void Map::create_house(const tmx::Object& object) {
    House house(object);
    const Rect& rect = house.rect;
    auto [left, right, top, bottom] = rect.get_position();
    int width = rect.get_width();

    //ajouter a la liste des portails
    list_portal.push_back(std::make_unique<House>(house));
}


