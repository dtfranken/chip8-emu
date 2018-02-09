#include <iostream>
#include "core.h"

/**
 * Initializes the core by setting up all registers and memory.
 */
void Core::initialize()
{
    srand(static_cast<unsigned int>(time(nullptr)));

    PC = PROGRAM_ADDRESS;
    SP = 0;
    I = 0;
    for (char i = 0; i < 16; ++i)
    {
        V[i] = 0;
        ram[i] = 0;
    }
    for (short i = 16; i < 4096; ++i)
    {
        ram[i] = 0;
    }
}

/**
 * Loads the specified program into memory.
 * @param program_name - the name of the program that will be loaded into memory.
 */
void Core::loadProgram(std::string program_name)
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
    in_reg_x = ram[PC] & static_cast<unsigned char>(0x0F);
    in_reg_y = ram[PC + 1] >> 4;
    in_constant_n = ram[PC + 1] & static_cast<unsigned char>(0x0F);
    in_address = in_reg_x << 8 | ram[PC + 1];

    switch (ram[PC] >> 4)
    {
        case 0x0:
            switch (in_address)
            {
                case 0x0E0:
                    // TODO: Clear display
                    break;
                case 0x0EE: // Return from subroutine
                    --SP;
                    {
                        short offset = STACK_ADDRESS + SP;
                        PC = ram[offset] << 8 | ram[offset + 1];
                    }
                    break;
                default:
                    // TODO: Call RCA 1802 program: rca(in_address)
                    break;
            }
            PC += 2;
            break;
        case 0x2: // Call subroutine at NNN
            {
                short offset = STACK_ADDRESS + SP;
                ram[offset] = static_cast<unsigned char>(PC >> 8);
                ram[offset + 1] = static_cast<unsigned char>(PC & 0x00FF);
            }
            ++SP;
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
                        auto msb = static_cast<unsigned char>(V[in_reg_y] >> 7);
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
            V[in_reg_x] = static_cast<unsigned char>(rand() % 256 & ram[PC + 1]);
            PC += 2;
            break;
        case 0xD:
            // TODO: Draw sprite at Vx, Vy
            break;
        case 0xE:
            switch (ram[PC+1])
            {
                case 0x9E:
                    // TODO: Handle key pressed
                    break;
                case 0xA1:
                    // TODO: Handle key not pressed
                    break;
                default:
                    PC += 2;
                    break;
            }
            break;
        case 0xF:
            // TODO: Implement
        default:
            break;
    }
}
