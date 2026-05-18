// Created by Rony on 5/16/2026.
// In this file, we are defining the functions and the constants for frontend.c

#ifndef C8EM_FRONTEND_H
#define C8EM_FRONTEND_H

#define WRONG_NUMBER_OF_ARGUMENTS 1
#define ROM_LOADED_SUCCESSFULLY 2
#define ROM_LOADING_ERROR 3
#define ROM_SIZE_TOO_BIG 4
#define GENERAL_FAILURE 5
#define SDL_INIT_ERROR 6
#define SDL_WINDOW_CREATION_ERROR 7
#define SDL_RENDERER_CREATION_ERROR 8
#define SDL_TEXTURE_CREATION_ERROR 9

#define CPU_SPEED 600
#define SCREEN_REFRESH_RATE 60
#include <stdint.h>

void load_rom(char *);

void emulate_cycle();

void update_timers();

void render_display();

void cleanup();

void wait_for_key_press();

void load_fonts();

#endif //C8EM_FRONTEND_H
