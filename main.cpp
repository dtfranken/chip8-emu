#include <iostream>
#include <chrono>
#include "core.h"
#include "include/SDL2/SDL.h"

int main(int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0){
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }
    SDL_Window* window = SDL_CreateWindow("CHIP-8 Emulator", SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED, Core::WIDTH << 4, Core::HEIGHT << 4, 0);
    if (window == nullptr)
    {
        std::cerr << "SDL_CreateWindow Failed: " << SDL_GetError() << std::endl;
        return 2;
    }
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr)
    {
        std::cerr << "SDL_CreateRenderer Failed: " << SDL_GetError() << std::endl;
        return 3;
    }
    SDL_Texture* screen = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB332,
            SDL_TEXTUREACCESS_STATIC, Core::WIDTH, Core::HEIGHT); // TODO: Stream instead of static access
    if (screen == nullptr)
    {
        std::cerr << "SDL_CreateTexture Failed: " << SDL_GetError() << std::endl;
        return 4;
    }


    Keyboard keyboard{};

    Timer delay_timer{};
    Timer sound_timer{};

    Core core{keyboard, delay_timer, sound_timer};

    // Initialize core, memory, timers and input
    core.initialize();
    core.loadProgram("../programs/octo.ch8");

    bool quit = false;
    SDL_Event e{};

    double preferred_cycle_duration = 1.0D / 500.0D;
    std::chrono::steady_clock::time_point end_prev_cycle{};
    std::chrono::duration<double> time_since_last_cycle{};

    double tick_duration = 1.0D / 60.0D;
    std::chrono::steady_clock::time_point prev_tick{};
    std::chrono::duration<double> time_since_last_tick{};

    // Emulation loop
    while (!quit)
    {
        time_since_last_cycle = std::chrono::steady_clock::now() - end_prev_cycle;
        if (time_since_last_cycle.count() >= preferred_cycle_duration)
        {
            core.emulateCycle();

            // Update screen if necessary
            if (core.draw_display) {
                /*for (auto i = 0; i < Core::RESOLUTION; ++i)
                {
                    std::cout << (core.getPixels()[i] ? "\uff04"  : "\uff0e");
                    if ((i + 1) % 64 == 0)
                        std::cout << "\n";
                }
                std::cout << "\n" << std::endl;*/

                // Update screen
                SDL_UpdateTexture(screen, nullptr, core.getPixels(), Core::WIDTH * sizeof(char));
                // TODO: Optimize drawing by only redrawing modified sections
                SDL_RenderClear(renderer);
                SDL_RenderCopy(renderer, screen, nullptr, nullptr);
                SDL_RenderPresent(renderer);

                core.draw_display = false;
                end_prev_cycle = std::chrono::steady_clock::now();
            }
        }

        // Update timers
        time_since_last_tick = std::chrono::steady_clock::now() - prev_tick;
        if (time_since_last_tick.count() >= tick_duration)
        {
            delay_timer.decrement();
            sound_timer.decrement();
            prev_tick = std::chrono::steady_clock::now();
        }

        if (sound_timer.getValue())
        {
            // TODO: beep();
        }

        // Update keyboard
        char key = -1;
        while (SDL_PollEvent(&e))
        {
            switch (e.type)
            {
                case SDL_KEYDOWN:
                case SDL_KEYUP:
                    switch (e.key.keysym.scancode)
                    {
                        case SDL_SCANCODE_1:
                        case SDL_SCANCODE_2:
                        case SDL_SCANCODE_3:
                            key = e.key.keysym.scancode - SDL_SCANCODE_1 + 0x1;
                            break;
                        case SDL_SCANCODE_4:
                            key = 0xC;
                            break;
                        case SDL_SCANCODE_Q:
                            key = 0x4;
                            break;
                        case SDL_SCANCODE_W:
                            key = 0x5;
                            break;
                        case SDL_SCANCODE_E:
                            key = 0x6;
                            break;
                        case SDL_SCANCODE_R:
                            key = 0xD;
                            break;
                        case SDL_SCANCODE_A:
                            key = 0x7;
                            break;
                        case SDL_SCANCODE_S:
                            key = 0x8;
                            break;
                        case SDL_SCANCODE_D:
                            key = 0x9;
                            break;
                        case SDL_SCANCODE_F:
                            key = 0xE;
                            break;
                        case SDL_SCANCODE_Z:
                            key = 0xA;
                            break;
                        case SDL_SCANCODE_X:
                            key = 0x0;
                            break;
                        case SDL_SCANCODE_C:
                            key = 0xB;
                            break;
                        case SDL_SCANCODE_V:
                            key = 0xF;
                        default:
                            break;
                    }
                    break;
                case SDL_QUIT:
                    quit = true;
                default:
                    break;
            }
            keyboard.setKey(key, e.key.state);
        }
    }

    // Clean up
    SDL_DestroyTexture(screen);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
