// Created by Rony on 5/16/2026.
// In this file, we are implementing the instruction set of the CHIP8 as per the technical reference document

#include "cpu.h"
#include "emu.h"

// This function clears the display. It does this by flipping all pixels to 0 //
void clear_display(Display* display) {
    // Loop through the horizontal pixels //
    for (int i=0; i<DISPLAY_HORIZONTAL_PIXELS; i++) {
        // For each horizontal pixel, loop through the vertical pixels //
        for (int j=0; j<DISPLAY_VERTICAL_PIXELS; j++) {
            // We AND the pixel (uint8) with 0x00 which is 0b00000000 to turn all the pixels off //
            display->display_screen[i][j] = display->display_screen[i][j] & 0x00;
        }
    }
    drawing_flag = 0xFF;
}

// This function is called when a program function returns, and it updates the program counter to the previous value,
// and it decrements the stack pointer by 1. Each time a program function is called, the CPU pushes the program counter
// into the stack and increments the stack pointer by 1. When it finishes the function execution, it pops up the old
// program counter value, load it into the program counter variable, and decrements the stack pointer by 1.
void subroutine_return(uint16_t program_counter, uint8_t* stack_pointer) {
    // Here we load the value pointed at by the stack pointer to the program counter //
    program_counter = *stack_pointer;
    // Now we decrement the stack pointer by 1 //
    stack_pointer -= 1;
}

// This function is used by the CPU to jump to a new memory address. How it works? It loads the desired jump address
// into the program counter. The new address is included in the operation code, that's why we are ANDing the opcode
// with 0x0FFF so we can extract the last 12-bits of the opcode. Each F (called nibble) in 0x0FFF is 4-bit.
void jump_function(uint16_t op_code, uint16_t program_counter) {
    // We AND the opcode with 0x0FFF and store the value inside the program counter //
    program_counter = op_code & 0x0FFF;
}

// This function calls a subroutine or a function at a specific address. The address is included in the last 12-bit of
// the operation code so we have to extract it by ANDing the opcode with 0x0FFF. But before loading the address into
// the program counter, we have to increment the stack pointer by 1, then retrieve the current program counter value
// and push it into the stack.
void call_subroutine(uint16_t op_code, uint16_t program_counter, uint8_t* stack_pointer) {
    // We increment the stack pointer by 1 //
    stack_pointer += 1;
    // We assign the variable pointed by the stack pointer the old program counter value //
    *stack_pointer = program_counter;
    // We extract the new function address and load it into the program counter //
    program_counter = op_code & 0x0FFF;
}

// This function is used to skip an instruction if the register Vx does not contain the same value as the one supplied
// in the last 8-bit of the operation code. So first, we have to get the value from the opcode, then compare it to the
// value of the Vx register, and skip an instruction if the values are the same (increase the program counter by 2).
void skip_next_instruction_if_Vx_equal_kk(uint16_t op_code, uint16_t program_counter, uint8_t* gp_registers) {
    // We extract the x value to locate the right register Vx. The x value is supplied in the second nibble
    // of the opcode. And since the Vx registers are 8-bit each, we AND the opcode with 0x0F000,
    // then we shift the opcode 8 bits to the right. Now we have the value of x. //
    uint8_t x = (uint8_t)((op_code & 0x0F00) >> 8);
    // We do the same to extract the value of kk. It is supplied with the opcode, using the last 2 nibbles //
    uint8_t kk_value = (uint8_t)(op_code & 0x00FF);
    // Now we compare the two values and increment the program counter by 2 if they are equal. //
    if (*(gp_registers + x) == kk_value) {
        program_counter += 2;
    }
}

// This function is the same as the one above, but if the values are not equal.
void skip_next_instruction_if_Vx_not_equal_kk(uint16_t op_code, uint16_t program_counter, uint8_t* gp_registers) {
    // Same as above. //
    uint8_t x = (uint8_t)((op_code & 0x0F00) >> 8);
    // Same as above. //
    uint8_t address = (uint8_t)(op_code & 0x00FF);\
    // We compare the two values and increment the program counter by 2 if they are not equal. //
    if (*(gp_registers + x) != address) {
        program_counter += 2;
    }
}

// This function compares the values of two registers Vx and Vy, and skip the next instruction if the values are equal.
// First, we have to extract the values of x and y supplied in the opcode's second and third nibbles.
void skip_next_instruction_if_Vx_equal_Vy(uint16_t op_code, uint16_t program_counter, uint8_t* gp_registers) {
    // We extract the x value to locate the right register Vx. The x value is supplied in the second nibble
    // of the opcode. And since the Vx registers are 8-bit each, we AND the opcode with 0x0F000,
    // then we shift the opcode 8 bits to the right. Now we have the value of x. //
    uint8_t x = (uint8_t)((op_code & 0x0F00) >> 8);
    // We do the same thing for the y value, but we shift it only 4 bits because it is the third nibble. //
    uint8_t y = (uint8_t)((op_code & 0x00F0) >> 4);
    // Check if the values are equal and increment the program counter by 2. //
    if (*(gp_registers + x) == *(gp_registers + y)) {
        program_counter += 2;
    }
}

// This functions loads a value supplied in the opcode into the register Vx. Both values (x and the other value) are
// included in the opcode in location 2,3 and 4 respectively. So we have to extract them first.
void load_byte_to_Vx(uint16_t op_code, uint8_t* gp_registers) {
    // Extract the value to be loaded from the operation code. //
    uint8_t value_to_be_loaded = (uint8_t)(op_code & 0x00FF);
    // Extract the register in which we have to store the value. //
    uint8_t x = (uint8_t)((op_code & 0x0F00) >> 8);
    // Load the value into the register. //
    *(gp_registers + x) = value_to_be_loaded;
}

// This function is the same as the one above but adds the values together and store them in the register.
// The function above replaced the value inside the register. This one adds them together then replace the old and
// the new value.
void add_byte_to_Vx(uint16_t op_code, uint8_t* gp_registers) {
    // We extract the value of x from the opcode. //
    uint8_t x = (uint8_t)((op_code & 0x0F00) >> 8);
    // We extract the value to be added from the opcode. //
    uint8_t value_to_be_added = (uint8_t)(op_code & 0x00FF);
    // Add the values together and store them inside the register. //
    *(gp_registers + x) += value_to_be_added;
}

// This function stores the value of the register Vy inside the register Vx. Both x and y are supplied in the opcode.
void store_Vy_in_Vx(uint16_t op_code, uint8_t* gp_registers) {
    // We extract the value of x from the operation code. //
    uint8_t x = (uint8_t)((op_code & 0x0F00) >> 8);
    // We extract the value of y from the operation code. //
    uint8_t y = (uint8_t)((op_code & 0x00F0) >> 4);
    // We store the value of register y inside register x. //
    *(gp_registers + x) = *(gp_registers + y);
}

// This function gets the values of the Vx and Vy registers, OR them together and store the result in register Vx.
void store_Vx_OR_Vy_in_Vx(uint16_t op_code, uint8_t* gp_registers) {
    // We extract the value of x from the operation code. //
    uint8_t x = (uint8_t)((op_code & 0x0F00) >> 8);
    // We extract the value of y from the operation code. //
    uint8_t y = (uint8_t)((op_code & 0x00F0) >> 4);
    // OR the values and store the result in register x. //
    *(gp_registers + x) = *(gp_registers + x) | *(gp_registers + y);
}

// This function gets the values of the Vx and Vy registers, AND them together and store the result in register Vx.
void store_Vx_AND_Vy_in_Vx(uint16_t op_code, uint8_t* gp_registers) {
    // We extract the value of x from the operation code. //
    uint8_t x = (uint8_t)((op_code & 0x0F00) >> 8);
    // We extract the value of y from the operation code. //
    uint8_t y = (uint8_t)((op_code & 0x00F0) >> 4);
    // AND the values and store the result in register x. //
    *(gp_registers + x) = *(gp_registers + x) & *(gp_registers + y);
}

// This function gets the values of the Vx and Vy registers, XOR them together and store the result in register Vx.
void store_Vx_XOR_Vy_in_Vx(uint16_t op_code, uint8_t* gp_registers) {
    // We extract the value of x from the operation code. //
    uint8_t x = (uint8_t)((op_code & 0x0F00) >> 8);
    // We extract the value of y from the operation code. //
    uint8_t y = (uint8_t)((op_code & 0x00F0) >> 4);
    // XOR the values and store the result in register x. //
    *(gp_registers + x) = *(gp_registers + x) ^ *(gp_registers + y);
}

// This function adds the value of Vy and Vx together, and set the register VF to 1 if the sum is greater than 255.
// The VF is set to 0 if the sum is less than 255. The lowest 8-bit of the sum are stored in Vx.
void add_Vy_to_Vx(uint16_t op_code, uint8_t* gp_registers) {
    // We extract the value of x from the operation code. //
    uint8_t x = (uint8_t)((op_code & 0x0F00) >> 8);
    // We extract the value of y from the operation code. //
    uint8_t y = (uint8_t)((op_code & 0x00F0) >> 4);
    // Calculate the sum of the values stored in registers Vx and Vy. //
    uint16_t sum = *(gp_registers + x) + *(gp_registers + y);
    // Store the lowest 8-bit of the sum in Vx //
    *(gp_registers + x) = (uint8_t)(sum | 0x00FF);
    // Check if sum is greater than 255, then set the register VF to 1. //
    *(gp_registers + VF) = sum > 255 ? 0xFF : 0x00;
}

// This function subtracts the value of Vy from Vx, and set the register VF to 1 if Vx is greater than Vy.
// The VF is set to 0 if Vx is less than Vy. The subtraction result is stored in Vx.
void sub_Vy_from_Vx(uint16_t op_code, uint8_t* gp_registers) {
    // We extract the value of x from the operation code. //
    uint8_t x = (uint8_t)((op_code & 0x0F00) >> 8);
    // We extract the value of y from the operation code. //
    uint8_t y = (uint8_t)((op_code & 0x00F0) >> 4);
    // Check if the value inside register Vx is greater than the value of register Vy. //
    if (*(gp_registers + x) > *(gp_registers + y)) {
        // Set the VF register to 1. //
        *(gp_registers + VF) = 0xFF;
    }
    else {
        // Set the VF register to 0. //
        *(gp_registers + VF) = 0x00;
    }
    // Store the result of the subtraction in Vx. //
    *(gp_registers + x) = *(gp_registers + x) - *(gp_registers + y);
}

// This function check if the lowest bit of Vx is 1. If yes, the VF register is set to 1, else VF is set to 0.
// The Vx register is then shifted 1-bit to the right (which means Vx is divided by 2).
void shr_Vx_by_1(uint16_t op_code, uint8_t* gp_registers) {
    // We extract the value of x from the operation code. //
    uint8_t x = (uint8_t)((op_code & 0x0F00) >> 8);
    // Here we are extracting the last bit of the Vx register value by ANDing it with 0x0001. //
    uint8_t lsb = *(gp_registers + x) & 0x01;
    // If the least significant bit is 1. //
    if (lsb == 1) {
        // Set the VF register to 1. //
        *(gp_registers + VF) = 0xFF;
    }
    else {
        // Set the VF register to 0. //
        *(gp_registers + VF) = 0x00;
    }
    // Shift the Vx value to the right by 1. //
    *(gp_registers + x) = *(gp_registers + x) >> 1;
}

// This function subtract Vx from Vy, and set the VF register to 1 if Vy is greater than Vx, and 0 if not.
// Then Vx is subtracted from Vy and the result is stored in Vx.
void subn_Vx_from_Vy(uint16_t op_code, uint8_t* gp_registers) {
    // We extract the value of x from the operation code. //
    uint8_t x = (uint8_t)((op_code & 0x0F00) >> 8);
    // We extract the value of y from the operation code. //
    uint8_t y = (uint8_t)((op_code & 0x00F0) >> 4);
    // Check if the value of Vy is greater than the value of Vx. //
    if (*(gp_registers + y) > *(gp_registers + x)) {
        // Set the VF flag to 1. //
        *(gp_registers + VF) = 0xFF;
    }
    else {
        // Set the VF flag to 0. //
        *(gp_registers + VF) = 0x00;
    }
    // Substract Vx from Vy and store the result in Vx. //
    *(gp_registers + x) = *(gp_registers + y) - *(gp_registers + x);
}

// This function check if the highest bit of Vx is 1. If yes, the VF register is set to 1, else VF is set to 0.
// The Vx register is then shifted 1-bit to the left (which means Vx is multiplied by 2).
void shl_Vx_by_1(uint16_t op_code, uint8_t* gp_registers) {
    // We extract the value of x from the operation code. //
    uint8_t x = (uint8_t)((op_code & 0x0F00) >> 8);
    // Here we are extracting the highest bit of the Vx register value by ANDing it with 0x80. //
    uint8_t msb = *(gp_registers + x) & 0x80;
    // If the most significant bit is 1. //
    if (msb == 0x80) {
        // Set the VF register to 1. //
        *(gp_registers + VF) = 0xFF;
    }
    else {
        // Set the VF register to 0. //
        *(gp_registers + VF) = 0x00;
    }
    // Shift the value of register Vx 1 bit to the left.
    *(gp_registers + x) = *(gp_registers + x) << 1;
}

// This function compares the value of Vx and Vy, and if they are not equal, the program counter is increased by 2.
void skip_next_instruction_if_Vx_not_equal_Vy(uint16_t op_code, uint8_t* gp_registers, uint16_t program_counter) {\
    // We extract the value of x from the operation code. //
    uint8_t x = (uint8_t)((op_code & 0x0F00) >> 8);
    // We extract the value of y from the operation code. //
    uint8_t y = (uint8_t)((op_code & 0x00F0) >> 4);
    // If the value of register Vx is not equal to the value of register Vy. //
    if (*(gp_registers + x) != *(gp_registers + y)) {
        // Increase the program counter value by 2. //
        program_counter += 2;
    }
}

// This function loads the address nnn supplied in the opcode to I register.
void set_I_to_nnn(uint16_t op_code, uint16_t I) {
    // We extract the value of nnn from the operation code. //
    uint16_t nnn = op_code & 0x0FFF;
    // We load the value of nnn into I. //
    I = nnn;
}

// This function jumps to location V0 + nnn.
void jump_to_V0_plus_nnn(uint16_t op_code, uint8_t* gp_registers, uint16_t program_counter) {
    // We extract the value of nnn from the operation code. //
    uint16_t nnn = op_code & 0x0FFF;
    // We set the program counter to the sum of the values of V0 and nnn. //
    program_counter = *(gp_registers + V0) + nnn;
}

// This function gets a randomly generated byte, AND it with the value of kk, then store the result in Vx.
void rnd_and_with_kk(uint16_t op_code, uint8_t* gp_registers, uint8_t random_byte) {
    // We extract the value of x from the operation code. //
    uint8_t x = (uint8_t)((op_code & 0x0F00) >> 8);
    // We extract the value of kk from the operation code. //
    uint8_t kk = (uint8_t)(op_code & 0x00FF);
    // AND the values of the random byte and kk and store the result in Vx. // n
    *(gp_registers + x) = random_byte & kk;
}

// This function checks if the pressed keyboard key is equal the value of Vx, then increments the program counter by 2.
void skip_next_instruction_if_Vx_equal_pressed_key(uint16_t op_code, uint8_t* gp_registers, uint16_t program_counter, uint16_t* keypad){
    // We extract the value of x from the operation code. //
    uint8_t x = (uint8_t)((op_code & 0x0F00) >> 8);
    // Loop through the keypad keys //
    for (int i=0;i<KEYPAD_SIZE;i++) {
        // Check if the key is pressed and the key is equal to the value of Vx. //
        if (keypad[i] == 0xFF && *(gp_registers + x) == keypad[i]) {
            // Then increment the program counter by 2. //
            program_counter += 2;
        }
    }
}

// This function checks if the key corresponding to the value of Vx is not pressed, then increments the program counter by 2.
void skip_next_instruction_if_Vx_not_equal_pressed_key(uint16_t op_code, uint8_t* gp_registers, uint16_t program_counter, uint16_t* keypad) {
    // We extract the value of x from the operation code. //
    uint8_t x = (uint8_t)((op_code & 0x0F00) >> 8);
    // Loop through the keypad keys. //
    for (int i=0;i<KEYPAD_SIZE;i++) {
        // Check if the key is not pressed and the key is equal to the value of Vx. //
        if (keypad[i] == 0x00 && *(gp_registers + x) == keypad[i]) {
            // Then increment the program counter by 2. //
            program_counter += 2;
        }
    }
}

// This function place the value of the delay timer in the Vx register.
void load_dt_in_Vx(uint16_t op_code, uint8_t* gp_registers, uint8_t sp_delay_timer) {
    // We extract the value of x from the operation code. //
    uint8_t x = (uint8_t)((op_code & 0x0F00) >> 8);
    // Store the delay timer value in the Vx register. //
    *(gp_registers + x) = sp_delay_timer;
}

// This function waits for a key press, and store the value of the key in Vx.
void store_pressed_key_in_Vx(uint16_t op_code, uint8_t* gp_registers) {
    // We extract the value of x from the operation code. //
    uint8_t x = (uint8_t)((op_code & 0x0F00) >> 8);
    // Raise the wait_for_key_press flag for the frontend. //
    wait_for_key_press = 0xFF;
    // Assign the value of the pressed key to Vx. //
    *(gp_registers + x) = pressed_key;
}

// This function loads the value of the Vx register into the delay timer register.
void load_Vx_in_DT(uint16_t op_code, uint8_t* gp_registers, uint8_t sp_delay_timer) {
    // We extract the value of x from the operation code. //
    uint8_t x = (uint8_t)((op_code & 0x0F00) >> 8);
    // Assign the value of Vx to the delay timer register. //
    sp_delay_timer = *(gp_registers + x);
}

// This function loads the value of the Vx register into the sound timer register.
void load_Vx_in_ST(uint16_t op_code, uint8_t* gp_registers, uint8_t sp_sound_timer) {
    // We extract the value of x from the operation code. //
    uint8_t x = (uint8_t)((op_code & 0x0F00) >> 8);
    // Assign the value of Vx to the sound timer register. //
    sp_sound_timer = *(gp_registers + x);
}

// This function adds the values of Vx and I registers, and store the result in the I register.
void add_Vx_to_I(uint16_t op_code, uint8_t* gp_registers, uint16_t I) {
    // We extract the value of x from the operation code. //
    uint8_t x = (uint8_t)((op_code & 0x0F00) >> 8);
    // Add the sum to I register. //
    I += *(gp_registers + x);
}

// This function takes the decimal value of Vx, and places the hundreds digit in memory
// at location in I, the tens digit at location I+1, and the ones digit at location I+2.
void store_bcd_of_Vx(uint16_t op_code, uint8_t* gp_registers, uint16_t I) {
    // We extract the value of x from the operation code. //
    uint8_t x = (uint8_t)((op_code & 0x0F00) >> 8);
    // Read the value stored in Vx. //
    uint8_t decimal = *(gp_registers + x);
    // store the hundreds digit. //
    I = (decimal / 100) % 10;
    // store the tens digit. //
    *(&I+1)= (decimal / 10) % 10;
    // store the ones digit. //
    *(&I+2)= decimal % 10;
}

// This function stores the values of registers V0 to Vx in memory starting at location I.
void store_V0_to_Vx_starting_I(uint16_t op_code, uint8_t* gp_registers, uint16_t I) {
    // We extract the value of x from the operation code. //
    uint8_t x = (uint8_t)((op_code & 0x0F00) >> 8);
    // Loop through the registers to get their values. //
    for (int i=0; i<=x; i++) {
        // Assign each register value to a memory location starting at I. //
        *(&I + i) = *(gp_registers + i);
    }
}

// This function loads the values from memory locations starting at I, and store the values in registers V0 to Vx.
void load_V0_to_Vx_starting_I(uint16_t op_code, uint8_t* gp_registers, uint16_t I) {
    // We extract the value of x from the operation code. //
    uint8_t x = (uint8_t)((op_code & 0x0F00) >> 8);
    // Loop through the registers to load their values. //
    for (int i=0; i<=x; i++) {
        // Assign each memory location to a register value starting at V0. //
        *(gp_registers + i) = *(&I + i);
    }
}

// This function reads n bytes from memory, starting at the address stored in I. These bytes are then displayed as
// sprites on screen at coordinates (Vx, Vy). Sprites are XORed onto the existing screen. If this causes any pixels
// to be erased, VF is set to 1, otherwise it is set to 0. If the sprite is positioned so part of it is outside the
// coordinates of the display, it wraps around to the opposite side of the screen
void display_n_sprite_at_I(uint16_t op_code, Display *display, uint8_t* gp_registers, uint8_t I) {
    // We extract the value of x from the operation code. //
    uint8_t x = (uint8_t)((op_code & 0x0F00) >> 8);
    // We extract the value of y from the operation code. //
    uint8_t y = (uint8_t)((op_code & 0x00F0) >> 4);
    // We extract the value of n from the operation code. //
    uint8_t n = (uint8_t)((op_code & 0x000F));
    // Declare an array to hold the sprite data read from memory. //
    uint8_t sprites[n];
    // Loop through data in memory. //
    for (int i=0; i<=n; i++) {
        // Add each memory location data to the array. //
        sprites[i] = *(&I + i);
    }
    // Loop through the read sprites. //
    for (int j=0; j<=n; j++) {
        // Get the current y position of the current sprite. //
        uint8_t current_y = y + j;

        // For each bit of the read sprite byte. //
        for (int k=0; k<=7; k++) {
            // Get the current x position of each and every bit. //
            uint8_t current_x = x + k;
            // Check if the current x position is outside the x-axis bounds. //
            if (current_x > DISPLAY_HORIZONTAL_PIXELS) {
                // If yes, this pixel should wrap around to the opposite side, so its x will become 0.
                current_x = 0;
                // And the y will increase by 1 because it wrapped to the opposite position. //
                current_y += 1;
            }
            // Check if the current y position is outside the x-axis bounds. //
            if (current_y > DISPLAY_VERTICAL_PIXELS) {
                // If yes, this pixel should wrap around to the opposite side, so its y will become 0.
                current_y = 0;
                // And the x will increase by 1 because it wrapped to the opposite position. //
                current_x += 1;
            }
            // Here we get the value or the state of the pixel (1 or 0). //
            uint8_t sprite_pixel = sprites[k] >> (7-k);
            // And we get the old pixel value. //
            uint8_t old_pixel_state = display->display_screen[current_x][current_y];
            // We XOR both valus and store the result at the same position. //
            display->display_screen[current_x][current_y] = old_pixel_state ^ sprite_pixel;
            // If the old pixel value was 1 and it flipped to 0. //
            if (old_pixel_state == 1 && display->display_screen[current_x][current_y] == 0) {
                // Then set the VF flag to 1. //
                *(gp_registers + VF) = 0xFF;
            }
            else {
                // Set the VF flag to 0. //
                *(gp_registers + VF) = 0x00;

            }
         }
    }
    drawing_flag = 0xFF;
}

// This function sets I register to the location of the hexadecimal sprite corresponding to the register Vx.
void store_Vx_loc_in_I(uint16_t op_code, Display *display, uint8_t I) {
    // We extract the value of x from the operation code. //
    uint8_t x = (uint8_t)((op_code & 0x0F00) >> 8);
    // Since each sprite is 5 bytes long, the first sprite starts at 0x50, and if we add 5*x we get the position //
    // of sprite corresponding to register Vx. //
    I = 0x50 + (x*5);
}