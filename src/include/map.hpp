#ifndef MAP_HPP
#define MAP_HPP

#include "main.hpp"

class Map {
public:
    int tileWidth;
    int tileHeight;

    tmx::Map map;
    vita2d_texture *tileset;
    

    Map();

    ~Map();

    void draw();
};



#endif