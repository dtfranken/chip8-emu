#ifndef CHIP8_EMU_CORE_H
#define CHIP8_EMU_CORE_H

#include <string>

/**
 * An implementation of the CHIP-8 core.
 */
class Core {

    static constexpr unsigned short PROGRAM_ADDRESS = 0x200;
    static constexpr unsigned short STACK_ADDRESS = 0xEA0;

    /**
     * Memory layout:
     *  0x000-0x1FF = Reserved
     *  0x200-0xE9F = Program
     *  0xEA0-0xEFF = Call Stack
     *  0xF00-0xFFF = Display Refresh
     */
    unsigned char ram[4096];

    /**
     * Registers:
     * - 16 general purpose registers, V0 to VF
     * - Stack pointer SP (24 levels)
     * - Address register I
     * - Program counter PC
     */
    unsigned char V[16];
    unsigned char SP;
    unsigned short I;
    unsigned short PC;

    /**
     * Display:
     * - Resolution = 64 x 32
     */
    unsigned char display[2048]; // TODO: Use bits instead of bytes

    /**
     * Timers:
     * - Delay @ 60 Hz
     * - Sound @ 60 Hz
     */
    unsigned char delay_timer;
    unsigned char sound_timer;

    /**
     * Input:
     * - 16 keys
     * - Set to 1 when pressed, 0 otherwise
     */
    unsigned char key[16]; // TODO: Use bits instead of bytes

    /**
     * Hold instruction data during execution:
     */
    unsigned short in_address;
    unsigned char in_constant_n;
    unsigned char in_reg_x;
    unsigned char in_reg_y;


public:
    void initialize();
    void loadProgram(std::string program_name);
    void emulateCycle();
};

#endif //CHIP8_EMU_CORE_H
