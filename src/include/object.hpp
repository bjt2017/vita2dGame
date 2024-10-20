#ifndef OBJECT_HPP
#define OBJECT_HPP

#define NORMAL 0
#define CUT 1
#define SELECT 2

class Tree{
public:
    int status=NORMAL;
    int x;
    int y;

    int width=32;
    int heigth=32;

    Tree(int x,int y,int couche);

    int draw_tile(int id);    
};

#endif