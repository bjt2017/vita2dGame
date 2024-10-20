#ifndef UTILS_HPP
#define UTILS_HPP

#include "globals.hpp"
#include "map.hpp"

class Map;

class Utils {
    public:
        static std::string split(std::string str, char delimiter, int index);
        static Tree* find_tree(int x, int y);
        static void update_list(Map *map);
};


#endif