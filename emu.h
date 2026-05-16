// Created by Rony on 5/16/2026.
// In this file we define the shared variables and structures needed for the backend and frontend communication

#ifndef C8EM_EMU_H
#define C8EM_EMU_H

// Here we define the display size //
#define DISPLAY_HORIZONTAL_PIXELS 64
#define DISPLAY_VERTICAL_PIXELS 32

// Here we define the max memory size //
#define MAX_MEMORY_SIZE 4096

// Here we define the program start location. (the address where the program is loaded) //
#define PROGRAM_START_LOCATION 512
#define PROGRAM_ETI_660_START_LOCATION 1536

// Here we define the total registers number //
#define GP_REGISTERS_NUMBERS 16

// Here we define the stack and keypad sizes //
#define STACK_MEMORY_SIZE 16
#define KEYPAD_SIZE 16

// Here we define the maximum allowed program size //
#define MAX_ROM_SIZE 3500

// Define general purpose registers names //
enum {
    V0 = 0,
    V1 = 1,
    V2 = 2,
    V3 = 3,
    V4 = 4,
    V5 = 5,
    V6 = 6,
    V7 = 7,
    V8 = 8,
    V9 = 9,
    VA = 10,
    VB = 11,
    VC = 12,
    VD = 13,
    VE = 14,
    VF = 15,
};

// Declare the display structure //
typedef struct {
    uint8_t display_screen[DISPLAY_HORIZONTAL_PIXELS][DISPLAY_VERTICAL_PIXELS];
}Display;

#endif //C8EM_EMU_H
