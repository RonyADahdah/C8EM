// Created by Rony on 5/16/2026.
// In this file, we are declaring the functions used in the frontend.
// This file will include all functions that are OS specific.

#include "frontend.h"

#include <stdio.h>

#include "cpu.h"
#include "emu.h"

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

    // Here we initialize the display structure. //
    Display display;

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
            rnd_and_with_kk(opcode, gp_registers);
            break;
        case 0xD000:
            display_n_sprite_at_I(opcode, display_ptr, gp_registers, I);
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
                    store_Vx_loc_in_I(opcode, &display, I);
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

}
