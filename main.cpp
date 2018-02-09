#include "core.h"
#include "keyboard.h"

int main()
{
    Keyboard keyboard{};
    Core core{keyboard};

    // Initialize registers
    core.initialize();
    core.loadProgram("../programs/IBM Logo.ch8");

    // Emulation loop
    while (true)
    {
        core.emulateCycle();

        // Update screen

        // Update keys
    }

    return 0;
}
