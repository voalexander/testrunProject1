#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fstream>
#include <string>
#include <signal.h>

#include "CPU.h"
#include "Memory.h"

// Main Driver Code

/*  
    Task for CPU
    Also handles the timer
    each iteration of the while look represents a single instruction
*/ 
void CPUTask(CPU * cpu, int * cpuToMem, int * memToCpu, int timerInterrupt)
{
    cpu->init(cpuToMem, memToCpu);
    int timer = 0; 
    while(true)
    {
        cpu->fetchInstruction(timer == timerInterrupt);
        if(cpu->processInstruction() == -1) // -1 = instruction 50 has been read
            exit(EXIT_SUCCESS);
        if(timer == timerInterrupt) //Reset timer after interrupt value reached
            timer = 0;
        timer += 1;
    }
}

/*  
    Task for Memory
    Handles the message processing for the memory
    The memory itself only reads and writes
*/
void MemTask(Memory * mem, int * cpuToMem, int * memToCpu)
{
    int msg;
    int i[3];
    void* inbuf;
    while(true)
    {
        if((msg = read(cpuToMem[0], &i, sizeof(i))) > 0)
        {
            switch(i[0])
            {
                case 0: // Read memory request
                {
                    int instr = mem->read(i[1]);
                    write(memToCpu[1], &instr, sizeof(instr));
                    break;
                }
                case 1: // Write memory request
                {
                    mem->write(i[1], i[2]);
                    break;
                }
                case -1: //Kill
                {
                    return;
                }
            }
        }
    }
}

// Reading file, parsing, and writing to memory
void loadFileToMem(Memory * mem, std::string filename)
{
    std::ifstream inputFile;

    inputFile.open(filename);
    if (inputFile.is_open())
    {
        std::string line;
        int LA = 0;
        while(std::getline(inputFile, line))
        {
            std::string numStr = "";
            bool newLA = false;

            for(int i = 0; i < line.length(); i++)
            {
                if(line[i] == '.')
                    newLA = true;
                else if(isdigit(line[i]))
                    numStr += line[i];
                else
                    break;
            }
            if(numStr.length() > 0)
            {
                if(newLA)
                    LA = stoi(numStr);
                else
                {
                    mem->write(LA, stoi(numStr));
                    LA += 1;
                }
            }
        }
    }
}

// Main driver
int main(int argc, char** argv)
{
    int timerInterrupt = 0;
    char* inputFileName = NULL;
    CPU cpu = CPU();
    Memory mem = Memory();
    int cpuToMem[2]; // CPU to Memory pipe
    int memToCpu[2]; // Memory to CPU pipe

    // Loading cmd arguments
    if (argc > 1)
    {
        inputFileName = argv[1];
        if(argc > 2)
            timerInterrupt = atoi(argv[2]);
        else
            timerInterrupt = INT32_MAX;
    }
    else
    {
        std::cout << "Error: Missing Parameters" << std::endl;
        return 0;
    }

    // Load file contents to memory
    loadFileToMem(&mem, inputFileName);

    // Init pipes
    if(pipe(cpuToMem) < 0 || pipe(memToCpu) < 0)
    {
        std::cout << "Pipe failure";
        exit(EXIT_FAILURE);
    }

    // Fork processes
    if(fork() > 0) // Main task timer
    {
        CPUTask(&cpu, cpuToMem, memToCpu, timerInterrupt);
    }
    else // Sub task memory
    {
        MemTask(&mem, cpuToMem, memToCpu);
    }
    return 0;
}