#ifndef CHIP8_EMU_CORE_H
#define CHIP8_EMU_CORE_H


/**
 * An implementation of the CHIP-8 core.
 */
class Core {

    const unsigned short STACK_ADDRESS = 0xEA0;

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
     */
    unsigned char key[16]; // TODO: Use bits instead of bytes

    unsigned short instruction;

};


#endif //CHIP8_EMU_CORE_H
