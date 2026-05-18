#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <Windows.h>

#include "frontend.h"
#include "emu.h"

uint8_t chip8_fontset[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

Display display;
uint8_t memory[MAX_MEMORY_SIZE];
uint8_t gp_registers[GP_REGISTERS_NUMBERS];
uint16_t I;
uint8_t sp_delay_timer, sp_sound_timer;
uint16_t program_counter;
uint8_t stack_pointer;
uint16_t stack_memory[STACK_MEMORY_SIZE];
uint16_t keypad[KEYPAD_SIZE];
uint8_t drawing_flag;
uint8_t wait_key_press;
uint8_t pressed_key;
char* program_name;

int main(int argc, char *argv[]) {
    if (argc != 3 && strcmp(argv[1],"ETI-660") != 0 && strcmp(argv[1],"NORMAL") != 0) {
        printf("Usage: ./C8EM mode romfile\n");
        printf("\t-mode: ETI-660 or NORMAL.\n");
        printf("\tromfile: the file path for the program to run.\n");
        exit(WRONG_NUMBER_OF_ARGUMENTS);
    }
    printf("+File name to be loaded: %s\n", argv[2]);

    // Zeroize all arrays. //
    memset(memory, 0, sizeof(memory));
    memset(gp_registers, 0, sizeof(gp_registers));
    memset(stack_memory, 0, sizeof(stack_memory));
    memset(keypad, 0, sizeof(keypad));
    memset(&display, 0, sizeof(Display));

    // Initialize all variables. //
    stack_pointer = 0x00;
    drawing_flag = 0x00;
    wait_key_press = 0x00;
    pressed_key = 0x00;

    // Load fonts data. //
    load_fonts();

    // Load the rom file. //
    load_rom(argv[2]);

    // Initialize timers. //
    sp_sound_timer = 0;
    sp_delay_timer = 0;

    // Starting the infinite loop. //
    while (1) {
        // Here we have to calculate how many instruction to execute per frame. Because the CPU is much faster,
        // than the screen, for each frame displayed multiple instructions can be executed.
        int instructions_per_frame = CPU_SPEED / SCREEN_REFRESH_RATE;

        // Start the clock. //
        clock_t start = clock();

        for (int i = 0; i < instructions_per_frame; i++) {
            emulate_cycle();

            if (wait_key_press == 0xFF) {
                wait_for_key_press();
            }
        }

        // Stop the clock. //
        clock_t end = clock();

        // Calculate the elapsed time. //
        clock_t diff = end - start;

        // Convert the clock result to double. //
        double elapsed_time = (double)diff / CLOCKS_PER_SEC;

        if (elapsed_time < 1000) {
            Sleep(1000-elapsed_time);
        }

        // Decrement the timers. //
        update_timers();

        // Check if the drawing flag is on. //
        if (drawing_flag == 0xFF) {
            render_display();
            drawing_flag = 0x00;
        }
    }

}