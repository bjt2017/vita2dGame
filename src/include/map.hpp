#ifndef MAP_HPP
#define MAP_HPP

#include "globals.hpp"
#include <tmxlite/Map.hpp>        
#include <tmxlite/TileLayer.hpp>  
#include <tmxlite/ObjectGroup.hpp> 
#include <vector>
#include "player.hpp"   
#include "console.hpp" 
#include "utils.hpp"
#include <psp2/kernel/processmgr.h>

class Player;

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
    
    Map();
    ~Map();
    void init(Player *player);
    void draw();
    void drawLayer(const tmx::TileLayer* layer,bool check);
    void drawTile(tmx::TileLayer::Tile tile,int x, int y,bool check);
    void move(int x, int y);

};




#endif