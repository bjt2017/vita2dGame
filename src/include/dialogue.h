#pragma once
#include <vita2d.h>
#include "ui/box.h"
#include "console.hpp"
#include <string>
#include <vector>
#include <psp2/kernel/threadmgr.h>
#include <cmath>
#include <psp2/ctrl.h>
#include <sstream>

struct DialogueStep {
    std::string name;
    std::string text; 
};

class DialogueManager {
public:
    void start(const std::vector<DialogueStep>& steps);
    void update(SceCtrlData& pad, const SceCtrlData& previousPad);
    void draw();
    void next();
    bool is_active() const { return active; }

private:
    float indicator_animation = 0;
    float indicator_animation_speed = -0.2f;
    UIBox dialogue_box = UIBox();
    std::vector<DialogueStep> current_dialogue;
    std::vector<std::string> wrap_text(const std::string& text, int maxWidth, float fontSize, bool truncate, bool& wasTruncated, std::string& remainingText);
    std::vector<std::string> wrapped_lines;
    size_t current_index = 0;
    bool active = false;
    std::string visible_text;  
    float type_timer = 0.0f;
    size_t char_index = 0;
    float char_speed = 0.03f;
};

extern DialogueManager dialogueManager;