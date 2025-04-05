#ifndef CONSOLE_HPP
#define CONSOLE_HPP

#include <vita2d.h>
#include <string>
#include <vector>

#define SCREEN_WIDTH 960
#define SCREEN_HEIGHT 544

class Console {
public:
    Console() = delete;
    ~Console() = delete;
    static int mode;
    static void init();
    static void clear();
    static void log(const std::string& message);
    static void warning(const std::string& message);
    static void show();
    static void shutdown();

private:
    static vita2d_pgf* font;
    static std::vector<std::string> logs;
    static std::vector<std::string> warnings;
    static bool ready;
    
};

#endif
