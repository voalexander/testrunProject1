#include "CPU.h"
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

// PC = Program Counter
// SP = Stack Pointer
// IR = Instruction Register
// AC = Accumulator

/* NOTE ALL FUNCTION DESCRIPTIONS ARE IN THE HEADER FILE */


void CPU::init(int * cpuToMemArg, int * memToCpuArg)
{
    this->cpuToMem = cpuToMemArg;
    this->memToCpu = memToCpuArg;
}

void CPU::enterInterrupt(InterruptType type)
{
    if(this->mode == USER)
    {
        this->SSP = this->SP;
        this->SP = 1999;
        if(type == SYS_CALL) //System Call
        {
            this->SPC = this->PC + 1;
            this->PC = 1499;
        }
        else if (type == TIMER) //Timer
        {
            this->SPC = this->PC;
            this->PC = 999;
        }
        this->mode = KERNEL;
    }
}

void CPU::exitInterrupt()
{
    if(this->mode == KERNEL)
    {
        this->SP = this->SSP;
        this->PC = this->SPC;
        this->mode = USER;
    }
}

void CPU::fetchInstruction(bool hasInterrupt)
{
    int i[2] = {0, PC};
    int msg;
    int instruction;

    if(hasInterrupt)
        this->enterInterrupt(TIMER);

    if(messageMem(i, &instruction, sizeof(i), sizeof(instruction)) != NULL)
    {
        if(instruction == 29)
        {
            this->enterInterrupt(SYS_CALL);
        }
        IR = instruction;
    }
    PC += 1;
}

int CPU::processInstruction()
{
    int operand;
    if(this->hasOperator)
    {
        operand = this->IR;
        this->IR = this->prevIR;
    }
    switch(this->IR) // Refer to Instruction Set for case functions
    {
        case 1:
        {
            if(this->hasOperator)
            {
                this->AC = operand;
                this->hasOperator = false;
            }
            else
                this->hasOperator = true;
            break;
        }
        case 2:
        {
            if(this->hasOperator) 
            {
                int i[2] = {0, operand};
                int result;
                if(messageMem(i, &result, sizeof(i), sizeof(result)) != NULL)
                    this->AC = result;
                this->hasOperator = false;
            }
            else
                this->hasOperator = true;
            break;
        }
        case 3:
        {
            if(this->hasOperator)
            {
                // Below reads address twice
                int i[2] = {0, operand};
                int result;
                int result2;
                if(messageMem(i, &result, sizeof(i), sizeof(result)) != NULL)
                {
                    i[1] = result;
                    if(messageMem(i, &result2, sizeof(i), sizeof(result2)) != NULL)
                        this->AC = result2;
                }
                this->hasOperator = false;
            }
            else
                this->hasOperator = true;
            break;
        }
        case 4:
        {
            if(this->hasOperator)
            {
                int i[2] = {0, operand + this->X};
                int result;
                if(messageMem(i, &result, sizeof(i), sizeof(result)) != NULL)
                    this->AC = result;
                this->hasOperator = false;
            }
            else
                this->hasOperator = true;
            break;
        }
        case 5:
        {
            if(this->hasOperator)
            {
                int i[2] = {0, operand + this->Y};
                int result;
                if(messageMem(i, &result, sizeof(i), sizeof(result)) != NULL)
                    this->AC = result;
                this->hasOperator = false;
            }
            else
                this->hasOperator = true;
            break;
        }
        case 6:
        {
            int i[2] = {0,this->SP + this->X};
            int result;
            if(messageMem(i, &result, sizeof(i), sizeof(result)) != NULL)
                this->AC = result;
            break;
        }
        case 7:
        {
            if(this->hasOperator)
            {
                int i[3] = {1, operand, this->AC};
                messageMem(i, NULL, sizeof(i), 0);
                this->hasOperator = false;
            }
            else
                this->hasOperator = true;
            break;
        }
        case 8:
        {
            srand(this->PC); // Randomize time based on PC
            this->AC = rand() % 100 + 1;
            break;
        }
        case 9:
        {
            if(this->hasOperator)
            {
                if(operand == 1)
                    std::cout << (int)this->AC;
                else if(operand == 2)
                    std::cout << char(this->AC);
                this->hasOperator = false;
            }
            else
                this->hasOperator = true;
            break;
        }
        case 10:
        {
            this->AC += this->X;
            break;
        }
        case 11:
        {
            this->AC += this->Y;
            break;
        }
        case 12:
        {
            this->AC -= this->X;
            break;
        }
        case 13:
        {
            this->AC -= this->Y;
            break;
        }
        case 14:
        {
            this->X = this->AC;
            break;
        }
        case 15:
        {
            this->AC = this->X;
            break;
        }
        case 16:
        {
            this->Y = this->AC;
            break;
        }
        case 17:
        {
            this->AC = this->Y;
            break;
        }
        case 18:
        {
            this->SP = this->AC;
            break;
        }
        case 19:
        {
            this->AC = this->SP;
            break;
        }
        case 20:
        {
            if(this->hasOperator)
            {
                this->PC = operand;
                this->hasOperator = false;
            }
            else
                this->hasOperator = true;
            break;
        }
        case 21:
        {
            if(this->hasOperator)
            {
                if(this->AC == 0)
                    this->PC = operand;
                this->hasOperator = false;
            }
            else
                this->hasOperator = true;
            break;
        }
        case 22:
        {
            if(this->hasOperator)
            {
                if(this->AC != 0)
                    this->PC = operand;
                this->hasOperator = false;
            }
            else
                this->hasOperator = true;
            break;
        }
        case 23:
        {
            if(this->hasOperator)
            {
                this->pushStack(this->PC);
                this->PC = operand;
                this->hasOperator = false;
            }
            else
                this->hasOperator = true;
            break;
        }
        case 24:
        {
            this->PC = this->popStack();
            break;
        }
        case 25:
        {
            this->X += 1;
            break;
        }
        case 26:
        {
            this->X -= 1;
            break;
        }
        case 27:
        {
            pushStack(this->AC);
            break;
        }
        case 28:
        {
            this->AC = popStack();
            break;
        }
        case 29: //System Call (handled elsewhere)
        {
            break;
        }
        case 30:
        {
            this->exitInterrupt();
            break;
        }
        case 50:
        {
            // Messages Memory task to end then kills itself
            int msg = -1;
            messageMem(&msg, NULL, sizeof(msg), 0);
            return -1;
            break;
        }
        default:
            break;
    }
    this->prevIR = this->IR;
    return 0;
}

void * CPU::messageMem(int * outMsg, int * inMsg, int sizeOut, int sizeIn)
{
    if(this->mode == USER && outMsg[1] > 999)
    {
        std::cout << "Memory violation: accessing system address in user mode" << std::endl;
        exit(EXIT_FAILURE);
    }
    write(this->cpuToMem[1], outMsg, sizeOut);
    if(inMsg != NULL)
    {
        if(read(this->memToCpu[0], inMsg, sizeIn) > 0)
        {
            return inMsg;
        }
    }
    return NULL;
}

void CPU::pushStack(int value)
{
    this->SP -= 1;
    int msg[3] = {1, this->SP, value};
    messageMem(msg, NULL, sizeof(msg), 0);
}

int CPU::popStack()
{
    int readMsg[2] = {0, this->SP};
    int writeMsg[3] = {1, this->SP, 0};
    int value;
    messageMem(readMsg, &value, sizeof(readMsg), sizeof(value)); // Read Value
    
    messageMem(writeMsg, NULL, sizeof(writeMsg), 0); // Set Value to 0
    this->SP += 1;
    return value;
}