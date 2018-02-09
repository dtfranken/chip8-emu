#include "keyboard.h"

void Keyboard::setKey(char key, bool pressed)
{
    auto keyMask = static_cast<short>(1 << key);
    if (pressed)
    {
        keys |= keyMask;
    }
    else
    {
        keys &= ~keyMask;
    }
}

bool Keyboard::getKey(char key)
{
    return static_cast<bool>((keys >> key) & 1);
}
