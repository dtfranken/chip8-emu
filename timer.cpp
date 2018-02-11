#include "timer.h"

/**
 * Returns the value of this timer.
 */
unsigned char Timer::getValue() const
{
    return value;
}

/**
 * Sets the value of this timer to the specified value.
 */
void Timer::setValue(unsigned char value)
{
    this->value = value;
}

/**
 * Decrements the value of this timer by 1, unless it is 0;
 */
void Timer::decrement()
{
    if (value > 0)
    {
        --value;
    }
}
