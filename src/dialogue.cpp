#include "include/dialogue.h"

const int HEIGHT_FONT = 24;

void DialogueManager::start(const std::vector<DialogueStep>& steps) {
    current_dialogue.clear();

    for (const auto& step : steps) {
        std::string remaining = step.text;
        bool truncated = false;
        std::string rest;

        do {
            DialogueStep newStep = step;
            newStep.text = remaining;

            bool wasTruncated = false;
            std::string extraText;
            wrap_text(remaining, SCREEN_WIDTH - 2 * 200 - 32, 24.0f, true, wasTruncated, extraText);

            if (wasTruncated) {
                newStep.text = remaining.substr(0, remaining.size() - extraText.size());
                remaining = extraText;
            } else {
                remaining = "";
            }

            current_dialogue.push_back(newStep);
            truncated = wasTruncated;
        } while (truncated);
    }

    current_index = 0;
    active = true;
    visible_text.clear();
    char_index = 0;
    type_timer = 0.0f;
    wrapped_lines.clear();
}


void DialogueManager::next(){
    if (!active || current_index >= current_dialogue.size()) return;

    current_index++;
    visible_text.clear();
    char_index = 0;
    type_timer = 0.0f;
    wrapped_lines.clear();

    if (current_index >= current_dialogue.size()) {
        active = false;
    }
}

void DialogueManager::update(SceCtrlData& pad, const SceCtrlData& previousPad) {
    if (!active || current_index >= current_dialogue.size()) return;

    if (pad.buttons & SCE_CTRL_CROSS && !(previousPad.buttons & SCE_CTRL_CROSS)) {
        if (char_index < current_dialogue[current_index].text.size()) {
            visible_text = current_dialogue[current_index].text;
            char_index = visible_text.size();
            bool truncated;
            std::string rest;
            wrapped_lines = wrap_text(visible_text, SCREEN_WIDTH - 2 * 200 - 32, 24.0f, true, truncated, rest);
        } else {
            next();
        }
    }

    const DialogueStep& step = current_dialogue[current_index];

    if (char_index < step.text.size()) {
        type_timer += 1.0f / 60.0f;
        if (type_timer >= char_speed) {
            visible_text += step.text[char_index];
            char_index++;
            type_timer = 0.0f;

            bool truncated;
            std::string rest;
            wrapped_lines = wrap_text(visible_text, SCREEN_WIDTH - 2 * 200 - 32, 24.0f, true, truncated, rest);
        }
    }
}


void DialogueManager::draw() {
    if (!active) return;

    constexpr uint32_t BOX_NO_BOTTOM = 
    BOX_TOP_LEFT | BOX_TOP | BOX_TOP_RIGHT |
    BOX_LEFT | BOX_CENTER | BOX_RIGHT ;

    const int margin_x = 200;
    const int dialogue_width = SCREEN_WIDTH - 2 * margin_x;
    const int name_box_width = 150;
    const int name_box_height = 45;
    const int dialogue_box_height = 100;
    const int scale = 4;

    const int position_name = SCREEN_HEIGHT - dialogue_box_height - name_box_height + 6;
    const int position_dialogue = SCREEN_HEIGHT - dialogue_box_height - 10;

    unsigned int name_box_color = RGBA8(230, 210, 190, 255);
    unsigned int dialogue_box_color = RGBA8(255, 255, 255, 255);
    unsigned int name_text_color = RGBA8(219, 169, 122, 255);
    unsigned int dialogue_text_color = RGBA8(23, 20, 36, 255);

    dialogue_box.draw(margin_x, position_dialogue, dialogue_width, dialogue_box_height, dialogue_box_color, scale);
    
    int text_y = position_dialogue + 30;
    for (const std::string& line : wrapped_lines) {
        vita2d_font_draw_text(Console::font, margin_x + 16, text_y, dialogue_text_color, 24.0f, line.c_str());
        text_y += HEIGHT_FONT;
    }

    dialogue_box.draw(margin_x, position_name, name_box_width, name_box_height, name_box_color, scale, BOX_NO_BOTTOM);
    vita2d_font_draw_text(Console::font, margin_x + 16, position_name + 26, name_text_color, 24.0f, current_dialogue[current_index].name.c_str());

    if (char_index == current_dialogue[current_index].text.size()) {
        float time_sec = sceKernelGetSystemTimeWide() / 1000000.0f;
        float offset = sinf(time_sec * 6.0f) * 3.0f;
        vita2d_draw_texture_scale(UIBox::indicator, 
            margin_x + dialogue_width - 40, 
            position_dialogue + dialogue_box_height - 16 * 2 + offset,
            3, 3);
    }
}


std::vector<std::string> DialogueManager::wrap_text(const std::string& text, int maxWidth, float fontSize, bool truncate, bool& wasTruncated, std::string& remainingText) {
    std::vector<std::string> lines;
    std::string currentLine;
    std::string word;
    std::istringstream iss(text);
    wasTruncated = false;
    remainingText.clear();
    const int MAX_LINES = 3;

    while (iss >> word) {
        std::string testLine = currentLine.empty() ? word : currentLine + " " + word;
        float lineWidth = vita2d_font_text_width(Console::font, fontSize, testLine.c_str());

        if (lineWidth > maxWidth) {
            if (!currentLine.empty())
                lines.push_back(currentLine);
            else
                lines.push_back(word); // si un mot est trop long seul

            currentLine = word;

            if (truncate && lines.size() >= MAX_LINES) {
                wasTruncated = true;
                break;
            }
        } else {
            currentLine = testLine;
        }

        if (truncate && lines.size() >= MAX_LINES) {
            wasTruncated = true;
            break;
        }
    }

    if (!wasTruncated && !currentLine.empty())
        lines.push_back(currentLine);
    else if (wasTruncated)
        remainingText = currentLine;

    if (wasTruncated && !lines.empty()) {
        if (lines.back().size() > 3)
            lines.back() = lines.back().substr(0, lines.back().size() - 3) + "...";
        else
            lines.back() += "...";
    }

    // Récupérer tout le texte restant
    std::string leftover;
    while (iss >> word) {
        if (!remainingText.empty()) remainingText += " ";
        remainingText += word;
    }

    if (!leftover.empty()) {
        if (!remainingText.empty()) remainingText += " ";
        remainingText += leftover;
    }

    return lines;
}
