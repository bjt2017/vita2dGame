#include "include/ui/box.h"

vita2d_texture* UIBox::top_left = nullptr;
vita2d_texture* UIBox::top = nullptr;
vita2d_texture* UIBox::top_right = nullptr;
vita2d_texture* UIBox::left = nullptr;
vita2d_texture* UIBox::center = nullptr;
vita2d_texture* UIBox::right = nullptr;
vita2d_texture* UIBox::bottom_left = nullptr;
vita2d_texture* UIBox::bottom = nullptr;
vita2d_texture* UIBox::bottom_right = nullptr;
vita2d_texture* UIBox::indicator = nullptr;
int UIBox::corner_w = 0;
int UIBox::corner_h = 0;


void UIBox::load_assets(){
    // Load textures from the specified folder path
    top_left = vita2d_load_PNG_buffer(&_binary_assets_UI_lt_box_9slice_tl_png_start);
    top = vita2d_load_PNG_buffer(&_binary_assets_UI_lt_box_9slice_tc_png_start);
    top_right = vita2d_load_PNG_buffer(&_binary_assets_UI_lt_box_9slice_tr_png_start);
    left = vita2d_load_PNG_buffer(&_binary_assets_UI_lt_box_9slice_lc_png_start);
    center = vita2d_load_PNG_buffer(&_binary_assets_UI_lt_box_9slice_c_png_start);
    right = vita2d_load_PNG_buffer(&_binary_assets_UI_lt_box_9slice_rc_png_start);
    bottom_left = vita2d_load_PNG_buffer(&_binary_assets_UI_lt_box_9slice_bl_png_start);
    bottom = vita2d_load_PNG_buffer(&_binary_assets_UI_lt_box_9slice_bc_png_start);
    bottom_right = vita2d_load_PNG_buffer(&_binary_assets_UI_lt_box_9slice_br_png_start);

    // Load indicator texture
    indicator = vita2d_load_PNG_buffer(&_binary_assets_UI_indicator_png_start);
    // Set corner dimensions based on loaded textures
    corner_w = vita2d_texture_get_width(top_left);
    corner_h = vita2d_texture_get_height(top_left);
}

void UIBox::free_assets() {
    if (top_left) vita2d_free_texture(top_left);
    if (top) vita2d_free_texture(top);
    if (top_right) vita2d_free_texture(top_right);
    if (left) vita2d_free_texture(left);
    if (center) vita2d_free_texture(center);
    if (right) vita2d_free_texture(right);
    if (bottom_left) vita2d_free_texture(bottom_left);
    if (bottom) vita2d_free_texture(bottom);
    if (bottom_right) vita2d_free_texture(bottom_right);
    if (indicator) vita2d_free_texture(indicator);
}

void UIBox::draw(int x, int y, int width, int height, uint32_t color, float scale, uint32_t parts) {
    if (!top_left || !top || !top_right || !left || !center || !right || !bottom_left || !bottom || !bottom_right)
        return;

    const int scaled_corner_w = corner_w * scale;
    const int scaled_corner_h = corner_h * scale;

    // Coins
    if (parts & BOX_TOP_LEFT)
        vita2d_draw_texture_tint_scale(top_left, x, y, scale, scale, color);
    if (parts & BOX_TOP_RIGHT)
        vita2d_draw_texture_tint_scale(top_right, x + width - scaled_corner_w, y, scale, scale, color);
    if (parts & BOX_BOTTOM_LEFT)
        vita2d_draw_texture_tint_scale(bottom_left, x, y + height - scaled_corner_h, scale, scale, color);
    if (parts & BOX_BOTTOM_RIGHT)
        vita2d_draw_texture_tint_scale(bottom_right, x + width - scaled_corner_w, y + height - scaled_corner_h, scale, scale, color);

    // Bords
    if (parts & BOX_TOP)
        vita2d_draw_texture_tint_scale(
            top,
            x + scaled_corner_w, y,
            (width - 2 * scaled_corner_w) / (float)vita2d_texture_get_width(top),
            scale, color
        );

    if (parts & BOX_BOTTOM)
        vita2d_draw_texture_tint_scale(
            bottom,
            x + scaled_corner_w, y + height - scaled_corner_h,
            (width - 2 * scaled_corner_w) / (float)vita2d_texture_get_width(bottom),
            scale, color
        );

    if (parts & BOX_LEFT)
        vita2d_draw_texture_tint_scale(
            left,
            x, y + scaled_corner_h,
            scale,
            (height - 2 * scaled_corner_h) / (float)vita2d_texture_get_height(left),
            color
        );

    if (parts & BOX_RIGHT)
        vita2d_draw_texture_tint_scale(
            right,
            x + width - scaled_corner_w, y + scaled_corner_h,
            scale,
            (height - 2 * scaled_corner_h) / (float)vita2d_texture_get_height(right),
            color
        );

    // Centre
    if (parts & BOX_CENTER)
        vita2d_draw_texture_tint_scale(
            center,
            x + scaled_corner_w, y + scaled_corner_h,
            (width - 2 * scaled_corner_w) / (float)vita2d_texture_get_width(center),
            (height - 2 * scaled_corner_h) / (float)vita2d_texture_get_height(center),
            color
        );
}