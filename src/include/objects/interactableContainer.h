#pragma once
#include "../types/rect.hpp"
#include "../dialogue.h"

enum class InteractableType {
    Dialogue,
    Cutscene,
    Item,
};

class InteractableContainer {
public:
    InteractableContainer(const Rect& boundingBox, InteractableType type, const std::string& name, const std::string& description)
        : boundingBox(boundingBox), type(type), name(name), description(description) {}
    ~InteractableContainer() = default;
    InteractableType getType() const { return type; }
    std::string getName() const { return name; }
    std::string getDescription() const { return description; }
    void setType(InteractableType newType) { type = newType; }
    void setName(const std::string& newName) { name = newName; }
    void setDescription(const std::string& newDescription) { description = newDescription; }
    virtual void interact() = 0;
    Rect getBoundingBox() const {
        return boundingBox;
    }
    void setBoundingBox(const Rect& boundingBox);

private:
    Rect boundingBox;
    InteractableType type;
    std::string name;
    std::string description;
};
