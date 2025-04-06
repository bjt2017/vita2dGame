// globals.hpp
#pragma once

#include <variant>
#include <vector>
#include <tmxlite/Object.hpp>
#include "console.hpp"
#include <sstream>
#include <unordered_set>
#include "objects/tree.hpp"

// Variables externes pour charger les assets des images
extern unsigned char _binary_assets_assets_png_start;

#define TILE_SIZE 16  // Taille d'une tuile
#define SCREEN_WIDTH 960  // Largeur de l'écran
#define SCREEN_HEIGHT 544  // Hauteur de l'écran

#define PLAYER_SPEED 1.5f

extern float ZOOM;


// Variables externes globales
using Shape = std::variant<Rect, Polygon>;
extern std::vector<Shape> list_collide_rect;

//extern std::vector<Rect> list_collide_rect;
extern std::vector<Tree> list_tree;

extern std::vector<Rect*> list_collide_rect_on_screen;
extern std::vector<Tree*> list_tree_on_screen;
