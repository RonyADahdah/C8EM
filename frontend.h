// Created by Rony on 5/16/2026.
// In this file, we are defining the functions and the constants for frontend.c

#ifndef C8EM_FRONTEND_H
#define C8EM_FRONTEND_H

#include <stdint.h>

void emulate_cycle();

void update_timers();

void render_display();

uint8_t wait_for_key_press();

#endif //C8EM_FRONTEND_H
