#include "include/object.hpp"

Tree::Tree(int x, int y,int couche){
    this->x=x;
    this->y=y;
}

int Tree::draw_tile(int id){
    if(this->status==NORMAL){
        return id;
    }else if(this->status==SELECT){
        return id+2;
    }
    return id;
}