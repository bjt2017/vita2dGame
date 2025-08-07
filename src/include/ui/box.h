#pragma once

#include <vita2d.h>
#include <string>
#include "../assets.h"


enum UIBoxParts : uint32_t {
    BOX_TOP_LEFT     = 1 << 0,
    BOX_TOP          = 1 << 1,
    BOX_TOP_RIGHT    = 1 << 2,
    BOX_LEFT         = 1 << 3,
    BOX_CENTER       = 1 << 4,
    BOX_RIGHT        = 1 << 5,
    BOX_BOTTOM_LEFT  = 1 << 6,
    BOX_BOTTOM       = 1 << 7,
    BOX_BOTTOM_RIGHT = 1 << 8,

    BOX_ALL = 0xFFFFFFFF
};

class UIBox {
public:
    static vita2d_texture* indicator;
    UIBox() = default;
    ~UIBox() = default;
    static void load_assets(); 
    static void free_assets();
    void draw(int x, int y, int width, int height, uint32_t color, float scale, uint32_t parts=BOX_ALL);

private:
    static vita2d_texture* top_left;
    static vita2d_texture* top;
    static vita2d_texture* top_right;
    static vita2d_texture* left;
    static vita2d_texture* center;
    static vita2d_texture* right;
    static vita2d_texture* bottom_left;
    static vita2d_texture* bottom;
    static vita2d_texture* bottom_right;

    void draw_texture_scaled(vita2d_texture* tex, int x, int y, int width, int height, uint32_t color);
    static int corner_w;
    static int corner_h;
};
