#include "keyboard.h"

/**
 * Sets the specified key to 1 if it is pressed, 0 otherwise.
 * @param key - the key to be set
 * @param pressed - whether the key is pressed or not
 */
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

/**
 * Determines whether the specified key is pressed.
 * @param key - the key to be checked
 * @return 1 if the specified key is pressed, 0 otherwise
 */
bool Keyboard::getKey(char key)
{
    return static_cast<bool>((keys >> key) & 1);
}

/**
 * Returns the first key found which is pressed.
 * @return the ID of a key which is pressed, or -1 if no key is pressed
 */
char Keyboard::getPressedKey()
{
    if (keys)
    {
        for (char i = 0; i < 16; ++i)
        {
            if (keys >> i & 1)
            {
                return i;
            }
        }
    }
    return -1;
}
