#include "include/console.hpp"

vita2d_pgf* Console::font = nullptr;
std::vector<std::string> Console::logs;
std::vector<std::string> Console::warnings;
bool Console::ready = false;
int Console::mode = 0;

void Console::init() {
    font = vita2d_load_default_pgf();
    ready = true;
    mode = 0;
}

void Console::log(const std::string& message) {
    if(!ready) return;
    logs.push_back(message);
}
void Console::warning(const std::string& message) {
    if(!ready) return;
    warnings.push_back("[!] :  " + message);
}

void Console::show() {
    if(!ready) return;
    int y = 40;
    int x = 20;
    if(mode==0){
        vita2d_draw_rectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, RGBA8(0, 0, 0, 200));
        for (auto& log : logs) {
            vita2d_pgf_draw_text(font, x, y, RGBA8(255, 255, 255, 255), 1.0f, log.c_str());
            y += 20;
        }
        for(auto& warning : warnings) {
            vita2d_pgf_draw_text(font, x, y, RGBA8(255, 165, 0, 255), 1.0f, warning.c_str());
            y += 20;
        }
    }else if(mode==1){
        vita2d_pgf_draw_text(font, 20, 20, RGBA8(255, 255, 255, 255), 1.0f, "Show collision mode");
    }
}

void Console::clear() {
    logs.clear();
    warnings.clear();
}

void Console::shutdown() {
    if (font) {
        vita2d_free_pgf(font);
        font = nullptr; 
    }
}

