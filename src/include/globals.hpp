// globals.hpp
#pragma once

#include <variant>
#include <vector>
#include <tmxlite/Object.hpp>
#include "console.hpp"
#include <sstream>
#include <unordered_set>
#include "objects/tree.hpp"
#include "cmath"
#include <memory>
#include <unordered_map>
#include <set>
#include <stack>

// Variables externes pour charger les assets des images
extern unsigned char _binary_assets_assets_png_start;

#define TILE_SIZE 16  // Taille d'une tuile
#define SCREEN_WIDTH 960  // Largeur de l'écran
#define SCREEN_HEIGHT 544  // Hauteur de l'écran

#define PLAYER_SPEED 1.5f

extern float ZOOM;

enum class PortalType {
    LayerChanger,
    MapChanger,
    House
};

struct Portal {
    Rect rect;
    PortalType type;
    Portal(int x, int y, int width, int height, PortalType type) : rect(x, y, width, height), type(type){}
    Portal(const tmx::Object& object, PortalType type) : rect(object), type(type){};
    virtual ~Portal() = default;
};

struct LayerChanger : public Portal {
    LayerChanger(int x, int y, int width, int height) : Portal(x, y, width, height, PortalType::LayerChanger) {}
    LayerChanger(const tmx::Object& object) : Portal(object, PortalType::LayerChanger) {}
};

struct MapChanger : public Portal {
    std::string value;
    MapChanger(int x, int y, int width, int height, const std::string& value = "") : Portal(x, y, width, height, PortalType::MapChanger), value(value) {}
    MapChanger(const tmx::Object& object, const std::string& value = "") : Portal(object, PortalType::MapChanger) , value(value) {}
};

struct House : public Portal {
    std::vector<std::pair<int, int>> roof_tiles_pos;
    House(int x, int y, int width, int height) : Portal(x, y, width, height, PortalType::House) {}
    House(const tmx::Object& object) : Portal(object, PortalType::House) {}
};

// Variables externes globales
using Shape = std::variant<Rect, Polygon>;

extern std::vector<Shape> list_collide_rect_global;
extern std::unordered_map<int, std::vector<Shape>> list_collide_rect_by_layer; // map de int (layer) vers vector de Shape
extern std::vector<Shape*> list_collide_rect;

extern std::vector<std::unique_ptr<Portal>> list_portal;
extern std::unordered_map<std::string, std::vector<Tree>> list_tree_by_map;
extern std::vector<Tree>* list_tree;



