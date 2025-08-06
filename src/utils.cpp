#include "include/utils.h"

std::string Utils::split(std::string str, char delimiter, int index) {
    std::string token;
    std::istringstream tokenStream(str);
    int i = 0;
    while (std::getline(tokenStream, token, delimiter)) {
        if(i == index){
            return token;
        }
        i++;
    }
    return "";
}

Tree* Utils::find_tree(int x, int y) {
    for (Tree& tree : *list_tree) {         
        if (x >= tree.get_position_x() && x < (tree.get_position_x() + tree.get_width()) && y >= tree.get_position_y() && y < (tree.get_position_y() + tree.get_height())) {
            return &tree;  
        }
    }
    return nullptr;  
}




