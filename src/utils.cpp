#include "include/utils.hpp"


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
    for (Tree& tree : list_tree) {         
        if (x >= tree.x && x < (tree.x + tree.width) && y >= tree.y && y < (tree.y + tree.heigth)) {
            return &tree;  
        }
    }
    return nullptr;  
}



void Utils::update_list(Map* map) {
    list_collide_objects_on_screen.clear();
    list_tree_on_screen.clear();

    float screenLeft = map->posX;
    float screenTop = map->posY;
    float screenRight = map->posX + SCREEN_WIDTH;
    float screenBottom = map->posY + SCREEN_HEIGHT;

    for (const auto& object : list_collide_objects) {
        float objLeft = object.getAABB().left * ZOOM;
        float objTop = object.getAABB().top * ZOOM;
        float objRight = objLeft + object.getAABB().width * ZOOM;
        float objBottom = objTop + object.getAABB().height * ZOOM;

        if (objRight >= screenLeft && objLeft <= screenRight &&
            objBottom >= screenTop && objTop <= screenBottom) {
            list_collide_objects_on_screen.push_back(object);
        }
    }

    for (const auto& tree : list_tree) {
        float treeLeft = tree.x * ZOOM;
        float treeTop = tree.y * ZOOM;
        float treeRight = treeLeft + tree.width * ZOOM;
        float treeBottom = treeTop + tree.heigth * ZOOM;

        if (treeRight >= screenLeft && treeLeft <= screenRight &&
            treeBottom >= screenTop && treeTop <= screenBottom) {
            list_tree_on_screen.push_back(tree);
        }
    }
}

