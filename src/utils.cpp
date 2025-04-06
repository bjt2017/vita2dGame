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
    for (Tree& tree : list_tree) {         
        if (x >= tree.get_position_x() && x < (tree.get_position_x() + tree.get_width()) && y >= tree.get_position_y() && y < (tree.get_position_y() + tree.get_height())) {
            return &tree;  
        }
    }
    return nullptr;  
}


//unused
void Utils::update_list() {

    //list_collide_rect_on_screen.clear();
    list_tree_on_screen.clear();

    list_collide_rect_on_screen.reserve(list_collide_rect.size());
    list_tree_on_screen.reserve(list_tree.size());

    // for (auto& object : list_collide_rect) {
    //     if (object.on_screen()) {
    //         list_collide_rect_on_screen.push_back(&object);  
    //     }
    // }

    for (auto& tree : list_tree) {
        if (tree.on_screen()) {
            list_tree_on_screen.push_back(&tree);  
        }
    }
}



