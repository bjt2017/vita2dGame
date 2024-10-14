#ifndef MAIN_HPP
#define MAIN_HPP

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <cstddef>

#include <tmxlite/Map.hpp>
#include <tmxlite/TileLayer.hpp>
#include <psp2/appmgr.h>

#include <psp2/ctrl.h>
#include <psp2/kernel/processmgr.h>

#include <vita2d.h>

#include <cstddef> 

#include "player.hpp"

// Variables externes pour charger les assets des images
extern unsigned char _binary_assets_assets_png_start;
extern unsigned char _binary_assets_base_walk_strip8_png_start;
extern unsigned char _binary_assets_base_idle_strip9_png_start;
extern unsigned char _binary_assets_bowlhair_idle_strip9_png_start;

// Définitions de constantes
#define IDLE 0
#define WALK 1

#define LEFT 0
#define RIGHT 1

#define TILE_SIZE 16  // Taille d'une tuile
#define SCREEN_WIDTH 960  // Largeur de l'écran
#define SCREEN_HEIGHT 544  // Hauteur de l'écran

#define PLAYER_SPRITE_X 40  // Position verticale du sprite de joueur dans l'image
#define PLAYER_SPRITE_Y 21  // Position horizontale du sprite de joueur dans l'image
#define PLAYER_SPRITE_WIDTH 16  // Largeur du sprite de joueur
#define PLAYER_SPRITE_HEIGHT 19  // Hauteur du sprite de joueur
#define PLAYER_SPRITE_SPACING 80  // Espace entre chaque sprite (horizontalement)

#define COLLIDE_ZONE_Y 13

#define ZOOM 2.0f

// Déclaration de la fonction pour dessiner la carte
void drawMap(const tmx::Map& map, vita2d_texture* tileset, int tileWidth, int tileHeight);

#endif 
