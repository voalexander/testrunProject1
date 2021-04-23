#include <fstream>


/*
    Memory class
    Only performs read and write operations
*/
class Memory
{
    public:
        int read(int address);
        void write(int address, int data);
    
    private:
        int mem[2000];
};