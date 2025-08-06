#ifndef MAP_HPP
#define MAP_HPP

#include "globals.hpp"
#include <tmxlite/Map.hpp>    
#include <tmxlite/TileLayer.hpp>  
#include <tmxlite/ObjectGroup.hpp> 
#include <tmxlite/LayerGroup.hpp>
#include <vector>
#include "objects/player.hpp"   
#include "console.hpp" 
#include "utils.h"
#include <psp2/kernel/processmgr.h>

class Player;

enum LayerType {
    TILE,
    TREE,
    WATER
};

class Map {
public:
    tmx::Map map;
    const tmx::LayerGroup* current_map=nullptr;
    std::string current_map_name;
    vita2d_texture *tileset;

    int tileWidth;
    int tileHeight;

    int mapWidth;
    int mapHeight;

    int posX;
    int posY;

    int firstTileX;
    int firstTileY;
    int lastTileX;
    int lastTileY;

    static float water_timer;
    static int water_frame;
    
    Map();
    ~Map();
    void init(Player &player);
    void change_map(const std::string& map_name,Player &player);
    static void update();
    void combine_collide_rect(const Player& player);
    void create_house(const tmx::Object& object);

    tmx::TileLayer& get_tile_layer(const std::string& layerName, const std::vector<std::string>& path = {}) const;
    const tmx::LayerGroup& get_tile_group(const std::vector<std::string>& path, const tmx::LayerGroup& currentGroup, size_t idx = 0) const;
    const tmx::LayerGroup& get_tile_group(const std::vector<std::string>& path, bool fromRoot) const;
    std::unordered_set<int> get_tiles_under_player(const Player& player,const std::string& layerName, const std::vector<std::string>& path={}) const;
    std::vector<std::pair<int,int>> get_adjacent_tiles(tmx::TileLayer layer, std::pair<int,int> pos);

    bool player_on_water(const Player& player) const;
    void draw_layer(const tmx::TileLayer& layer,LayerType type);
    void draw_tile(tmx::TileLayer::Tile tile,int x, int y,LayerType type);
    void move(int x, int y);
    const tmx::LayerGroup& getCurrentMap() const { return *current_map; }
};

#endif