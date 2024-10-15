#ifndef CONSOLE_HPP
#define CONSOLE_HPP

#include "main.hpp"

class Console {
    public:
        Console();
        ~Console();
        void clear();
        void log(std::string message);
        void show();
    private:
        vita2d_pgf* font;
        std::vector<std::string> logs;
};




#endif