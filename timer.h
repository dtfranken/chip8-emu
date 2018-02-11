#ifndef CHIP8_EMU_TIMER_H
#define CHIP8_EMU_TIMER_H

/**
 * This class represents a CHIP-8 timer, which has a value that can be set, read and decremented by 1.
 */
class Timer {
    unsigned char value;

public:
    void setValue(unsigned char value);
    unsigned char getValue() const;
    void decrement();
};

#endif //CHIP8_EMU_TIMER_H
