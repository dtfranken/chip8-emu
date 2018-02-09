#ifndef CHIP8_EMU_KEYBOARD_H
#define CHIP8_EMU_KEYBOARD_H


class Keyboard
{
    /**
     * 16 keys
     */
    short keys = 0;

public:
    void setKey(char key, bool pressed);
    bool getKey(char key);
};


#endif //CHIP8_EMU_KEYBOARD_H
