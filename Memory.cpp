#include "Memory.h"

int Memory::read(int address)
{
    return this->mem[address];
}

void Memory::write(int address, int data)
{
    this->mem[address] = data;
}
