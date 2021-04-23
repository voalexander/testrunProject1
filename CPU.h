#define register int

// Enum defining the interrupt type
enum InterruptType
{
    TIMER,
    SYS_CALL
};

// Enum defining the current Cpu mode
enum CpuMode
{
    USER,
    KERNEL
};

/*
    CPU class
    Processes all instructions read from memory
    Includes registers and local variables used for other tasks
*/
class CPU
{
    public:
        /*
            Initialization function
            Saves a pointer from both pipes
        */
        void init(int * cpuToMemArg, int * memToCpuArg);

        /*
            Fetches instructions from memory
            Also detects interrupts and performs the according action
        */
        void fetchInstruction(bool hasInterrupt);

        /*
            Processes the instruction read from memory
            Returns 0 for all instructions except 50
            Returns -1 upon receiving instruction 50
        */
        int processInstruction();

    private:
        /*
            Helper function used to message the memory task
            Returns void pointer of the result (if any)
            Also detects memory violations
        */
        void * messageMem(int * outMsg, int * inMsg, int sizeOut, int sizeIn);
        
        /*
            Helper function that enters the interrupt
            Saves the previous SP and PC
            Moves the SP according to the interrupt type
            Switches the CPU mode to kernel
            Does not activate if already in kernel mode
        */
        void enterInterrupt(InterruptType type);

        /*
            Helper function that exits the interrupt state
            Switches SP and PC back to the saved values
            Switches CPU back to user mode
            Only activates if CPU is in kernel mode
        */
        void exitInterrupt();

        /*
            Helper function that pushes to the stack
            Pushes by messaging the memory
        */
        void pushStack(int value);

        /*
            Helper function that pops from the stack
            Pops by messaging the memory
        */
        int popStack();

        /* Variables */

        CpuMode mode;               // Stores current CPU mode
        int * cpuToMem;             // Cpu to memory pipe
        int * memToCpu;             // Memory to Cpu pipe
        register PC = 0;            // Program Counter register
        register SP = 1000;         // Stack pointer register
        register IR;                // Instruction register
        register AC = 0;            // Accumulator
        register X = 0;             // General register X
        register Y = 0;             // General register Y
        bool hasOperator = false;   // Helper- keeps track if next instruction
                                    // is an operand
        int prevIR;                 // Helper- keeps track of previous
                                    // instruction for use with operand
        int SSP;                    // saved stack pointer
        int SPC;                    // saved program counter
};      
