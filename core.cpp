#include <iostream>
#include "core.h"

Core::Core(Keyboard& keyboard, Timer& delay_timer, Timer& sound_timer) : keyboard(keyboard),
        delay_timer(delay_timer), sound_timer(sound_timer) {}

unsigned char* Core::getPixels()
{
    return display;
}

/**
 * Initializes the core by setting up all registers and memory.
 */
void Core::initialize()
{
    srand(static_cast<unsigned int>(time(nullptr)));

    delay_timer.setValue(0);
    sound_timer.setValue(0);

    draw_display = false;

    PC = PROGRAM_ADDRESS;
    SP = 0;
    I = 0;
    for (char i = 0; i < 16; ++i)
    {
        V[i] = 0;
        display[i] = 0;
    }
    for (char i = 16; i < FONT_ADDRESS + 80; ++i) // 5 bytes per character, 16 characters = 80 bytes
    {
        display[i] = 0;
    }
    for (short i = FONT_ADDRESS + 5 * 0xF; i < RESOLUTION; ++i)
    {
        display[i] = 0;
        ram[i] = 0;
    }
    for (short i = RESOLUTION; i < 4096; ++i)
    {
        ram[i] = 0;
    }

    // Load font data into memory
    for (char i = 0; i < 80; ++i)
    {
        ram[i] = font_data[i];
    }
}

/**
 * Loads the specified program into memory.
 * @param program_name - the name of the program that will be loaded into memory.
 */
void Core::loadProgram(const std::string& program_name)
{
    FILE * program = std::fopen(program_name.c_str(), "rb");
    if (!program)
    {
        std::cerr << "ERROR: File " << program_name << " could not be read." << std::endl;
        throw(errno);
    }

    std::fseek(program, 0, SEEK_END);
    long program_size = std::ftell(program);
    if (program_size > STACK_ADDRESS - PROGRAM_ADDRESS)
    {
        std::cerr << "ERROR: File " << program_name << " is too large." << std::endl;
        errno = ENOMEM;
        throw(errno);
    }

    std::rewind(program);
    std::fread(&ram[PROGRAM_ADDRESS], 1, static_cast<size_t>(program_size), program);
    std::fclose(program);
}

/**
 * Emulates one cycle.
 */
void Core::emulateCycle()
{
    //std::printf("Instruction: %X\n", ram[PC] << 8 | ram[PC+1]);

    in_reg_x = ram[PC] & static_cast<unsigned char>(0x0F);
    in_reg_y = ram[PC + 1] >> 4;
    in_constant_n = ram[PC + 1] & static_cast<unsigned char>(0x0F);
    in_address = in_reg_x << 8 | ram[PC + 1];

    switch (ram[PC] >> 4)
    {
        case 0x0:
            switch (in_address)
            {
                case 0x0E0: // Clear display
                    for (short i = 0; i < RESOLUTION; ++i)
                    {
                        display[i] = 0;
                    }
                    draw_display = true;
                    break;
                case 0x0EE: // Return from subroutine
                    SP -= 2;
                    {
                        short address = STACK_ADDRESS + SP;
                        PC = ram[address] << 8 | ram[address + 1];
                    }
                    break;
                default:
                    // TODO: Call RCA 1802 program: rca(in_address)
                    std::printf("Call to RCA 1802 program at 0x%X.\n", in_address);
                    break;
            }
            PC += 2;
            break;
        case 0x2: // Call subroutine at NNN
            {
                short address = STACK_ADDRESS + SP;
                ram[address] = static_cast<unsigned char>(PC >> 8);
                ram[address + 1] = static_cast<unsigned char>(PC & 0x00FF);
            }
            SP += 2;
        case 0x1: // Jump to address NNN
            PC = in_address;
            break;
        case 0x3: // Skip the next instruction if Vx == NN
            PC += (V[in_reg_x] == ram[PC+1]) ? 4 : 2;
            break;
        case 0x4: // Skip the next instruction if Vx != NN
            PC += (V[in_reg_x] != ram[PC+1]) ? 4 : 2;
            break;
        case 0x5:
            if (in_constant_n) // Invalid opcode
            {
                PC += 2;
            }
            else // Skip the next instruction if Vx == Vy
            {
                PC += (V[in_reg_x] == V[in_reg_y]) ? 4 : 2;
            }
            break;
        case 0x6: // Set Vx to NN
            V[in_reg_x] = ram[PC+1];
            PC += 2;
            break;
        case 0x7: // Add NN to Vx (no carry flag)
            V[in_reg_x] += ram[PC+1];
            PC += 2;
            break;
        case 0x8:
            switch (in_constant_n)
            {
                case 0x0: // Set Vx to Vy
                    V[in_reg_x] = V[in_reg_y];
                    break;
                case 0x1: // Set Vx to Vx OR Vy
                    V[in_reg_x] |= V[in_reg_y];
                    break;
                case 0x2: // Set Vx to Vx AND Vy
                    V[in_reg_x] &= V[in_reg_y];
                    break;
                case 0x3: // Set Vx to Vx XOR Vy
                    V[in_reg_x] ^= V[in_reg_y];
                    break;
                case 0x4: // Add Vy to Vx (set carry flag VF to 1 on carry, 0 otherwise)
                    {
                        unsigned short sum = V[in_reg_x] + V[in_reg_y];
                        V[in_reg_x] = static_cast<unsigned char>(sum);
                        V[0xF] = static_cast<unsigned char>((sum > 0xFF) ? 1 : 0);
                    }
                    break;
                case 0x5: // Subtract Vy from Vx (set borrow flag VF to 0 on borrow, 1 otherwise)
                    {
                        unsigned short diff = V[in_reg_x] - V[in_reg_y];
                        V[in_reg_x] = static_cast<unsigned char>(diff);
                        V[0xF] = static_cast<unsigned char>((diff > 0xFF) ? 0 : 1);
                    }
                    break;
                case 0x6: // Set VF to Vy & 1, set Vx = Vy = Vy >> 1
                    {
                        auto lsb = static_cast<unsigned char>(V[in_reg_y] & 1);
                        V[in_reg_x] = V[in_reg_y] >>= 1;
                        V[0xF] = lsb;
                    }
                    break;
                case 0x7: // Set Vx to Vy - Vx (set borrow flag VF to 0 on borrow, 1 otherwise)
                    {
                        unsigned short diff = V[in_reg_y] - V[in_reg_x];
                        V[in_reg_x] = static_cast<unsigned char>(diff);
                        V[0xF] = static_cast<unsigned char>((diff > 0xFF) ? 0 : 1);
                    }
                    break;
                case 0xE: // Set VF to Vy >> 7, set Vx = Vy = Vy << 1
                    {
                        unsigned char msb = V[in_reg_y] >> 7;
                        V[in_reg_x] = V[in_reg_y] <<= 1;
                        V[0xF] = msb;
                    }
                default:
                    break;
            }
            PC += 2;
            break;
        case 0x9: // Skip the next instruction if Vx != Vy
            PC += (V[in_reg_x] != V[in_reg_y]) ? 4 : 2;
            break;
        case 0xA: // Set I = NNN
            I = in_address;
            PC += 2;
            break;
        case 0xB: // Jump to address NNN + V0
            PC = in_address + V[0];
            break;
        case 0xC: // Set Vx = NN & random number
            V[in_reg_x] = static_cast<unsigned char>(rand() & ram[PC + 1]);
            PC += 2;
            break;
        case 0xD: // Draw a sprite at Vx, Vy, 8 pixels wide and N pixels high, which is stored at I
            {
                V[0xF] = 0;
                unsigned char pixel_row;
                short pixel_index;
                unsigned char pixel_data;
                for (unsigned char row = 0; row < in_constant_n; ++row)
                {
                    pixel_row = ram[I+row];
                    for (unsigned char col = 0; col < 8; ++col)
                    {
                        pixel_data = static_cast<unsigned char>((pixel_row >> (7 - col) & 1) ? -1 : 0);
                        pixel_index = V[in_reg_x] + col + (V[in_reg_y] + row) * WIDTH;
                        display[pixel_index] ^= pixel_data;
                        if (!V[0xF])
                        {
                            V[0xF] |= static_cast<unsigned char>(pixel_data & ~display[pixel_index] ? 1 : 0); // Set collision flag VF to 1 if a pixel is unset
                        }
                    }
                }
                //std::cout << "Collision Flag VF = " << (V[0xF] ? '1' : '0') << std::endl;
            }
            draw_display = true;
            PC += 2;
            break;
        case 0xE:
            switch (ram[PC+1])
            {
                case 0x9E: // Skip the next instruction if the key stored in Vx is pressed
                    PC += keyboard.getKey(V[in_reg_x]) ? 4 : 2;
                    break;
                case 0xA1: // Skip the next instruction if the key stored in Vx is not pressed
                    PC += keyboard.getKey(V[in_reg_x]) ? 2 : 4;
                    break;
                default:
                    PC += 2;
                    break;
            }
            break;
        case 0xF:
            switch (ram[PC+1])
            {
                case 0x07: // Set Vx to the value of the delay timer
                    V[in_reg_x] = delay_timer.getValue();
                    break;
                case 0x0A: // Halt program execution until a key is pressed, and store the key in Vx
                    {
                        char key = keyboard.getPressedKey();
                        if (keyboard.getPressedKey() < 0)
                        {
                            return;
                        }
                        V[in_reg_x] = static_cast<unsigned char>(key);
                    }
                    break;
                case 0x15: // Set delay timer to Vx
                    delay_timer.setValue(V[in_reg_x]);
                    break;
                case 0x18: // Set sound timer to Vx
                    sound_timer.setValue(V[in_reg_x]);
                    break;
                case 0x1E: // Add Vx to I (set carry flag VF to 1 on carry, 0 otherwise)
                    I += V[in_reg_x];
                    if (I > 0xFFF)
                    {
                        I &= 0xFFF;
                        V[0xF] = 1;
                    }
                    else
                    {
                        V[0xF] = 0;
                    }
                    break;
                case 0x29: // Set I to the address of the font for the character in Vx
                    I = static_cast<unsigned short>(FONT_ADDRESS + 5 * V[in_reg_x]);
                    break;
                case 0x33: // Store the BCD representation of Vx at address I, I+1, I+2
                    ram[I] = static_cast<unsigned char>((V[in_reg_x] >> 2) / 25);
                    ram[I+1] = static_cast<unsigned char>(V[in_reg_x] / 10 % 10);
                    ram[I+2] = static_cast<unsigned char>(V[in_reg_x] % 10);
                    break;
                case 0x55: // Store V0 to Vx at address I to I+x
                    for (int reg = 0; reg <= in_reg_x; ++reg)
                    {
                        ram[I] = V[reg];
                        ++I;
                    }
                    break;
                case 0x65: // Load values stored at address I to I+x into V0 to Vx
                    for (int reg = 0; reg <= in_reg_x; ++reg)
                    {
                        V[reg] = ram[I];
                        ++I;
                    }
                default: // Invalid opcode
                    break;
            }
        default:
            PC += 2;
            break;
    }
}
