#include "include/console.hpp"

Console::Console() {
    ready = false;
}

Console::~Console() {
    vita2d_free_pgf(font);
}

void Console::init() {
    font = vita2d_load_default_pgf();
    ready = true;
}


void Console::log(std::string message) {
    if(!ready) return;
    logs.push_back(message);
}

void Console::show() {
    if(!ready) return;
    int y = 40;
    int x = 20;
    vita2d_draw_rectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, RGBA8(0, 0, 0, 200));
    for (auto& log : logs) {
        vita2d_pgf_draw_text(font, x, y, RGBA8(255, 255, 255, 255), 1.0f, log.c_str());
        y += 20;
    }
}

void Console::clear() {
    logs.clear();
}

