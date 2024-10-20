// globals.hpp

#ifndef GLOBALS_HPP
#define GLOBALS_HPP

#include <vector>
#include <tmxlite/Object.hpp>
#include "console.hpp"
#include <sstream>
#include "object.hpp"
#include <unordered_set>

// Variables externes pour charger les assets des images
extern unsigned char _binary_assets_assets_png_start;

extern unsigned char _binary_assets_Characters_Idle_base_png_start;
extern unsigned char _binary_assets_Characters_Idle_bowlhair_png_start;
extern unsigned char _binary_assets_Characters_Idle_tools_png_start;

extern unsigned char _binary_assets_Characters_Walk_base_png_start;
extern unsigned char _binary_assets_Characters_Walk_bowlhair_png_start;
extern unsigned char _binary_assets_Characters_Walk_tools_png_start;

extern unsigned char _binary_assets_Characters_Axe_base_png_start;
extern unsigned char _binary_assets_Characters_Axe_bowlhair_png_start;
extern unsigned char _binary_assets_Characters_Axe_tools_png_start;

// Définitions de constantes
#define IDLE 0
#define WALK 1
#define Axe 2

#define LEFT 0
#define RIGHT 1

#define DOWN 0
#define UP 1

#define TILE_SIZE 16  // Taille d'une tuile
#define SCREEN_WIDTH 960  // Largeur de l'écran
#define SCREEN_HEIGHT 544  // Hauteur de l'écran

#define PLAYER_SPRITE_WIDTH 96  
#define PLAYER_SPRITE_HEIGHT 64 

#define PLAYER_REEL_POS_X 42
#define PLAYER_REEL_POS_Y 21

#define PLAYER_REEL_WIDTH 13
#define PLAYER_REEL_HEIGHT 18

#define PLAYER_SPEED 1.5f

#define COLLIDE_ZONE_Y 11

extern float ZOOM;

// Variables externes globales
extern Console console;
extern std::vector<tmx::Object> list_collide_objects;
extern std::vector<Tree> list_tree;

extern std::vector<tmx::Object> list_collide_objects_on_screen;
extern std::vector<Tree> list_tree_on_screen;

#endif
