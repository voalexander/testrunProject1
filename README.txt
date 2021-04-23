List of files

- main.cpp (Main driver)
- CPU.h
- CPU.cpp  (CPU Class)
- Memory.h
- Memory.cpp (Memory Class)
- sample5.txt

To compile on CS1, compile as usual with all 'cpp' files with the additional flag '-std=c++1'

Example:
g++ main.cpp CPU.cpp Memory.cpp -o MYOUTPUT -std=c++11

Run the file with the following params
./myfile {instruction set filename} {timer interrupt number}


The instrution set filename is required and the interrupt number is optional. If no interrupt number is added then a default of INT_MAX is used.



The sample file should output a sentence. The output will vary for different timer interrupt numbers as well. Please try with no timer interrupts first.