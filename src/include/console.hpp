#ifndef CONSOLE_HPP
#define CONSOLE_HPP

#include <vita2d.h>
#include <string>
#include <vector>

#define SCREEN_WIDTH 960
#define SCREEN_HEIGHT 544

class Console {
    public:
        Console();
        ~Console();
        void init();
        void clear();
        void log(std::string message);
        void show();
    private:
        vita2d_pgf* font;
        std::vector<std::string> logs;
        bool ready = false;
};




#endif