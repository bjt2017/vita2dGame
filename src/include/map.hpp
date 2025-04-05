#ifndef MAP_HPP
#define MAP_HPP

#include "globals.hpp"
#include <tmxlite/Map.hpp>        
#include <tmxlite/TileLayer.hpp>  
#include <tmxlite/ObjectGroup.hpp> 
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
    static void update();
    void draw_layer(const tmx::TileLayer& layer,LayerType type);
    void draw_tile(tmx::TileLayer::Tile tile,int x, int y,LayerType type);
    void move(int x, int y);
};




#endif