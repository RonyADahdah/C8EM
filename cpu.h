//
// Created by Rony-PC on 5/16/2026.
//

#ifndef C8EM_CPU_H
#define C8EM_CPU_H

// Created by Rony on 5/16/2026.
// In this file, we are declaring the instruction set of the CHIP8 as per the technical reference document

void clear_display(Display* display);

void subroutine_return(uint16_t program_counter, uint8_t* stack_pointer);

void call_subroutine(uint16_t op_code, uint16_t program_counter, uint8_t* stack_pointer);

void skip_next_instruction_if_Vx_equal_kk(uint16_t op_code, uint16_t program_counter, uint8_t* gp_registers);

void skip_next_instruction_if_Vx_not_equal_kk(uint16_t op_code, uint16_t program_counter, uint8_t* gp_registers);

void skip_next_instruction_if_Vx_equal_Vy(uint16_t op_code, uint16_t program_counter, uint8_t* gp_registers);

void load_byte_to_Vx(uint16_t op_code, uint8_t* gp_registers);

void add_byte_to_Vx(uint16_t op_code, uint8_t* gp_registers);

void store_Vy_in_Vx(uint16_t op_code, uint8_t* gp_registers);

void store_Vx_OR_Vy_in_Vx(uint16_t op_code, uint8_t* gp_registers);

void store_Vx_AND_Vy_in_Vx(uint16_t op_code, uint8_t* gp_registers);

void store_Vx_XOR_Vy_in_Vx(uint16_t op_code, uint8_t* gp_registers);

void add_Vy_to_Vx(uint16_t op_code, uint8_t* gp_registers);

void sub_Vy_from_Vx(uint16_t op_code, uint8_t* gp_registers);

void shr_Vx_by_1(uint16_t op_code, uint8_t* gp_registers);

void subn_Vx_from_Vy(uint16_t op_code, uint8_t* gp_registers);

void shl_Vx_by_1(uint16_t op_code, uint8_t* gp_registers);

void skip_next_instruction_if_Vx_not_equal_Vy(uint16_t op_code, uint8_t* gp_registers, uint16_t program_counter);

void set_I_to_nnn(uint16_t op_code, uint16_t I);

void jump_to_V0_plus_nnn(uint16_t op_code, uint8_t* gp_registers, uint16_t program_counter);

void rnd_and_with_kk(uint16_t op_code, uint8_t* gp_registers, uint8_t random_byte);

void skip_next_instruction_if_Vx_equal_pressed_key(uint16_t op_code, uint8_t* gp_registers, uint16_t program_counter, uint16_t* keypad);

void skip_next_instruction_if_Vx_not_equal_pressed_key(uint16_t op_code, uint8_t* gp_registers, uint16_t program_counter, uint16_t* keypad);

void load_dt_in_Vx(uint16_t op_code, uint8_t* gp_registers, uint8_t sp_delay_timer);

void store_pressed_key_in_Vx(uint16_t op_code, uint8_t* gp_registers);

void load_Vx_in_DT(uint16_t op_code, uint8_t* gp_registers, uint8_t sp_delay_timer);

void load_Vx_in_ST(uint16_t op_code, uint8_t* gp_registers, uint8_t sp_sound_timer);

void add_Vx_to_I(uint16_t op_code, uint8_t* gp_registers, uint16_t I);

void store_bcd_of_Vx(uint16_t op_code, uint8_t* gp_registers, uint16_t I);

void store_V0_to_Vx_starting_I(uint16_t op_code, uint8_t* gp_registers, uint16_t I);

void load_V0_to_Vx_starting_I(uint16_t op_code, uint8_t* gp_registers, uint16_t I);

void display_n_sprite_at_I(uint16_t op_code, Display *display, uint8_t* gp_registers, uint8_t I);

void store_Vx_loc_in_I(uint16_t op_code, Display *display, uint8_t I);

#endif //C8EM_CPU_H
