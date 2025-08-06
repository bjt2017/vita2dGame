#pragma once
#include "globals.hpp"

class Utils {
    public:
        static std::string split(std::string str, char delimiter, int index);
        static Tree* find_tree(int x, int y);
};