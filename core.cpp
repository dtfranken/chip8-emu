#include <iostream>
#include "core.h"

/**
 * Initializes the core by setting up all registers and memory.
 */
void Core::initialize()
{
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
    // Fetch instruction

    // Decode instruction

    // Execute instruction
}
