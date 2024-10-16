#ifndef MAP_HPP
#define MAP_HPP

#include "main.hpp"

class Map {
public:
    tmx::Map map;
    vita2d_texture *tileset;
    
    Map();

    ~Map();

    void draw();
    void move(int x, int y, Player *player);

private:
    int tileWidth;
    int tileHeight;

    int mapWidth;
    int mapHeight;

    int posX;
    int posY;

};



#endif