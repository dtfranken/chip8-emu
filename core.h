#ifndef CHIP8_EMU_CORE_H
#define CHIP8_EMU_CORE_H

#include "keyboard.h"
#include "timer.h"
#include <string>

/**
 * An implementation of the CHIP-8 core.
 */
class Core
{
public:
    static constexpr char WIDTH = 64;
    static constexpr char HEIGHT = 32;
    static constexpr short RESOLUTION = WIDTH * HEIGHT;
private:
    static constexpr unsigned short FONT_ADDRESS = 0x000;
    static constexpr unsigned short PROGRAM_ADDRESS = 0x200;
    static constexpr unsigned short STACK_ADDRESS = 0xEA0;

    /**
     * The CHIP-8 font that is loaded into memory during initialization.
     * Contains sprites for the characters 0-9 and A-F.
     */
    unsigned char font_data[80] =
    {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x90, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xF0, 0x90, 0xE0, 0x90, 0xF0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };

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
     * Monochrome Display:
     * - Resolution = 64 x 32
     */
    unsigned char display[RESOLUTION];

    /**
     * 2 Timers:
     * - Delay timer
     * - Sound timer
     */
    Timer& delay_timer;
    Timer& sound_timer;

    /**
     * Input:
     * - 16 keys
     * - Set to 1 when pressed, 0 otherwise
     */
    Keyboard& keyboard;

    /**
     * Hold instruction data during execution:
     */
    unsigned short in_address;
    unsigned char in_constant_n;
    unsigned char in_reg_x;
    unsigned char in_reg_y;

public:
    Core(Keyboard& keyboard, Timer& delay_timer, Timer& sound_timer);
    void initialize();
    void loadProgram(const std::string& program_name);
    void emulateCycle();
    unsigned char* getPixels();

    /**
     * Flag that indicates whether the screen needs to be redrawn.
     */
    bool draw_display;
};

#endif //CHIP8_EMU_CORE_H
