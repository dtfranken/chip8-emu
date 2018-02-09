#include "core.h"

int main()
{
    Core core{};

    // Initialize registers
    core.initialize();
    core.loadProgram("../programs/IBM Logo.ch8");

    // Emulation loop
    while (true)
    {
        core.emulateCycle();

        // Update screen
    }

    return 0;
}
