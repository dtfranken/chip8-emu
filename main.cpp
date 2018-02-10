#include <iostream>
#include "core.h"
#include "include/SDL2/SDL.h"

int main(int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0){
        std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }
    // TODO: SDL stuff


    Keyboard keyboard{};
    Core core{keyboard};

    // Initialize registers
    core.initialize();
    core.loadProgram("../programs/IBM Logo.ch8");

    bool quit = false;
    // Emulation loop
    while (!quit)
    {
        core.emulateCycle();

        if (core.draw_display)
        {
            /*for (auto i = 0; i < 32 * 64; ++i)
            {
                std::cout << (core.getPixels()[i] ? "\uff04"  : "\uff0e");
                if ((i + 1) % 64 == 0)
                    std::cout << "\n";
            }
            std::cout << std::endl;*/

            // Update screen

            // TODO: Draw screen
            // TODO: Optimize drawing by only redrawing modified sections
            core.draw_display = false;
        }

        // Update keys

        // Update timers
    }

    return 0;
}
