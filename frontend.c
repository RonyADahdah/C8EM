// Created by Rony on 5/16/2026.
// In this file, we are declaring the functions used in the frontend.
// This file will include all functions that are OS specific.

#include "frontend.h"
#include "cpu.h"
#include "emu.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_render.h>

extern Display display;
extern uint8_t chip8_fontset[80];

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* texture = NULL;

void* texture_data = NULL;
int pitch = 0;

// This function loads the program into the memory.
void load_rom(char *rom_file) {
    // Open the program rom file for read. //
    FILE *fp = fopen(rom_file, "rb");
    // Check if the file cannot be opened. //
    if (!fp) {
        // Print an error message and exit. //
        printf("Could not open rom file\n");
        exit(ROM_LOADING_ERROR);
    }
    // If the file was opened successfully, then move the file pointer to the end of the file. //
    fseek(fp, 0, SEEK_END);

    // Get the file size. //
    long filesize = ftell(fp);

    // Check if the loaded file size is greater than the allowed size. //
    if (filesize > MAX_ROM_SIZE) {
        // If yes, close the file, print an error message then exit. //
        fclose(fp);
        printf("ROM file is too big\n");
        exit(ROM_SIZE_TOO_BIG);
    }
    // If the file size is correct, print the file size. //
    printf("+File size is %ld\n", filesize);

    // Then move the file pointer back to the beginning of the file. //
    fseek(fp, 0, SEEK_SET);

    // Read the data from the file into the memory starting at location PROGRAM_START_LOCATION. //
    size_t read_count = fread(&memory[PROGRAM_START_LOCATION], sizeof(uint8_t), filesize, fp);
    // Print an informational message. //
    printf("+Read %llu bytes from loaded file\n", read_count);
    // Check if the data read is not equal to the file size. //
    if (read_count != filesize) {
        // If not, close the file, print an error message then exit. //
        fclose(fp);
        printf("Something went wrong!\n");
        exit(GENERAL_FAILURE);
    }
    // If nothing goes wrong, print a success message and set the program counter.
    printf("+ROM was loaded successfully!\n");
    printf("+Setting initial program counter\n");
    program_counter = PROGRAM_START_LOCATION;

    // Tokenize the argument to get the program name. //
    program_name = strtok(rom_file, "/");
    while (program_name != NULL) {
        program_name = strtok(NULL, "/");
    }

    printf("+Program counter: %u\n", program_counter);
}

void emulate_cycle() {
    // Get the operation code from the program counter. //
    uint16_t opcode = memory[program_counter];
    // The program counter has a size of 1 byte, while the operation code has a size of 2. //
    // Each program counter read represents a part of the opcode. //
    // For example if we read program_counter[0], we get the highest order byte. //
    // And if we read program_counter[1], we get the lowest order byte. //
    // Then we combine the two to get the opcode. //
    // Here we are shifting the first read to the left by 1 byte to combine it with the other 1 byte on the right. //
    opcode = (opcode << 8) | memory[program_counter + 1];

    // For debugging purposes. //
    printf("+Opcode: %02x\n", opcode);

    // Here we increase the program counter for the next cycle. //
    program_counter += 2;

    // Here begins the switch statement that compare the obtained opcode with the instruction set values. //
    switch (opcode & 0xF000) {
        case 0x0000:
            switch (opcode & 0x000F) {
                case 0x0000:
                    clear_display(&display);
                    break;
                case 0x000E:
                    subroutine_return(program_counter, stack_pointer);
                    break;
                default:
                    break;
            }
        case 0x1000:
            jump_function(opcode, program_counter);
            break;
        case 0x2000:
            call_subroutine(opcode, program_counter, stack_pointer);
            break;
        case 0x3000:
            skip_next_instruction_if_Vx_equal_kk(opcode, program_counter, gp_registers);
            break;
        case 0x4000:
            skip_next_instruction_if_Vx_not_equal_kk(opcode, program_counter, gp_registers);
            break;
        case 0x5000:
            skip_next_instruction_if_Vx_equal_Vy(opcode, program_counter, gp_registers);
            break;
        case 0x6000:
            load_byte_to_Vx(opcode, gp_registers);
            break;
        case 0x7000:
            add_byte_to_Vx(opcode, gp_registers);
            break;
        case 0x8000:
            switch (opcode & 0x000F) {
                case 0x0000:
                    store_Vy_in_Vx(opcode, gp_registers);
                    break;
                case 0x0001:
                    store_Vx_OR_Vy_in_Vx(opcode, gp_registers);
                    break;
                case 0x0002:
                    store_Vx_AND_Vy_in_Vx(opcode, gp_registers);
                    break;
                case 0x0003:
                    store_Vx_XOR_Vy_in_Vx(opcode, gp_registers);
                    break;
                case 0x0004:
                    add_Vy_to_Vx(opcode, gp_registers);
                    break;
                case 0x0005:
                    sub_Vy_from_Vx(opcode, gp_registers);
                    break;
                case 0x0006:
                    shr_Vx_by_1(opcode, gp_registers);
                    break;
                case 0x0007:
                    subn_Vx_from_Vy(opcode, gp_registers);
                    break;
                case 0x000E:
                    shl_Vx_by_1(opcode, gp_registers);
                    break;
                default:
                    break;
            }
        case 0x9000:
            skip_next_instruction_if_Vx_not_equal_Vy(opcode, gp_registers, program_counter);
            break;
        case 0xA000:
            set_I_to_nnn(opcode, I);
            break;
        case 0xB000:
            jump_to_V0_plus_nnn(opcode, gp_registers, program_counter);
            break;
        case 0xC000:
            uint8_t random_byte = (uint8_t)(rand()%256);
            rnd_and_with_kk(opcode, gp_registers, random_byte);
            break;
        case 0xD000:
            display_n_sprite_at_I(opcode, &display, gp_registers, I);
            break;
        case 0xE000:
            switch (opcode & 0x000F) {
                case 0x000E:
                    skip_next_instruction_if_Vx_equal_pressed_key(opcode, gp_registers, program_counter, &keypad[0]);
                    break;
                case 0x0001:
                    skip_next_instruction_if_Vx_not_equal_pressed_key(opcode, gp_registers, program_counter, &keypad[0]);
                    break;
                default:
                    break;
            }
        case 0xF000:
            switch (opcode & 0x000F) {
                case 0x0007:
                    load_dt_in_Vx(opcode, gp_registers, sp_delay_timer);
                    break;
                case 0x000A:
                    store_pressed_key_in_Vx(opcode, gp_registers);
                    break;
                case 0x0008:
                    load_Vx_in_ST(opcode, gp_registers, sp_sound_timer);
                    break;
                case 0x000E:
                    add_Vx_to_I(opcode, gp_registers, I);
                    break;
                case 0x0009:
                    store_Vx_loc_in_I(opcode, I);
                    break;
                case 0x0003:
                    store_bcd_of_Vx(opcode, gp_registers, I);
                    break;
                case 0x0005:
                    switch (opcode & 0x00F0) {
                        case 0x0010:
                            load_Vx_in_DT(opcode, gp_registers, sp_delay_timer);
                            break;
                        case 0x0050:
                            store_V0_to_Vx_starting_I(opcode, gp_registers, I);
                            break;
                        case 0x0060:
                            load_V0_to_Vx_starting_I(opcode, gp_registers, I);
                            break;
                        default:
                            break;
                    }
                default:
                    break;
            }
        default:
            break;
    }
}

void update_timers() {
    sp_delay_timer -= 1;
    sp_sound_timer -= 1;
}

void render_display() {
    // Check if SDL3 is initialized or not. //
    if (!SDL_InitSubSystem(SDL_INIT_VIDEO)) {
        // If not print an error message then exit. //
        printf("+Couldn't initialize SDL3.\n");
        exit(SDL_INIT_ERROR);
    }

    // Check if the window was not previously created. //
    if (window == NULL) {
        // Create a window for display. //
        window = SDL_CreateWindow(program_name, DISPLAY_HORIZONTAL_PIXELS, DISPLAY_VERTICAL_PIXELS,
        SDL_WINDOW_MAXIMIZED);

        // Check if the window was not created successfully. //
        if (!window) {
            // Print an error code then exit. //
            printf("Couldn't create window.\n");
            exit(SDL_WINDOW_CREATION_ERROR);
        }
    }

    // Check if the renderer was not previously created. //
    if (renderer == NULL) {
        // Try to create the renderer to display our memory on screen. //
        renderer = SDL_CreateRenderer(window, NULL);

        // If the renderer was not created successfully. //
        if (!renderer) {
            // Print an error message then exit. //
            printf("Couldn't create renderer.\n");
            exit(SDL_RENDERER_CREATION_ERROR);
        }
    }

    // Check if the texture was previously created. //
    if (texture == NULL) {
        // Try to create the texture for rendering context. //
        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_XRGB8888, SDL_TEXTUREACCESS_STREAMING,
            DISPLAY_HORIZONTAL_PIXELS, DISPLAY_VERTICAL_PIXELS);

        // If the texture was not created successfully. //
        if (!texture) {
            printf("Couldn't create texture.\n");
            exit(SDL_TEXTURE_CREATION_ERROR);
        }

        // Try to lock the entire texture to copy frame data. //
        if (SDL_LockTexture(texture, NULL, &texture_data, &pitch) == true) {
            // Calculate the pitch data. //
            pitch = DISPLAY_HORIZONTAL_PIXELS * 8;
            Display* display_ptr = &display;
            printf("display ptr: %p\n", display_ptr);
            printf("texture data: %p\n", texture_data);
            printf("pitch: %d\n", pitch);
            // Copy the data from our memory to the GPU memory. //
            memcpy(texture_data, display_ptr, pitch);
        }

        drawing_flag = 0x00;
        // Render the data. //
        SDL_UnlockTexture(texture);

        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
    }



}

void cleanup() {
    // Destroy created window. //
    SDL_DestroyWindow(window);

    // Shutdown initialized subsystems. //
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

void wait_for_key_press() {
    // Create an event. //
    SDL_Event event;
    // Check if pressed_key variable is 0. //
    while (!pressed_key) {
        // If yes, wait for a press event. //
        SDL_WaitEvent(&event);
        // If the event type is a pressed button. //
        if (event.type == SDL_EVENT_KEY_DOWN) {
            // Raise the pressed flag. //
            pressed_key = 1;
        }
    }
}

void load_fonts() {
    // Create a pointer to hold the start address for the fonts in memory. //
    uint8_t font_start_address = 0x000;
    memcpy(&memory[font_start_address], chip8_fontset, sizeof(chip8_fontset));
}