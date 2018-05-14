Requirements:

1. gcc version 5.4.0 20160609
2. Ubuntu 16.04

Instructions for compilation:

1. To compile the source file "main.cpp" : make main
2. To remove the extra files obtained after compiling: make clean

Instrucions for use:

1. Run the program using command: ./main
2. Provide the size of RAM, size of page/frame, number of harddisk in the beginning.

Following commands are implemented in the simulation:

    A                       ‘A’ input means that a new process has been created. 
    Q                       The currently running process has spent a time quantum using the CPU.
    t                       The process that is currently using the CPU terminates. It leaves the system immediately.
    d number file_name      The process that currently uses the CPU requests the hard disk #number. It wants to read or write file file_name.
    D number                The hard disk #number has finished the work for one process.
    m address               The process that is currently using the CPU requests a memory operation for the logical address.
    S r                     Shows what process is currently using the CPU and what processes are waiting in the ready-queue. 
    S i                     Shows what processes are currently using the hard disks and what processes are waiting to use them. For each busy hard disk show the process that uses it and show its I/O-queue. Make sure to display the filenames (from the d command) for each process. The enumeration of hard disks starts from 0.
    S m                     Shows the state of memory. For each used frame display the process number that occupies it and the page number stored in it. The enumeration of pages and frames starts from 0.

NOTE:
1. logical address in binary format and the last n bits are offset_bits if  n = (ram_size/page_size).
2. ram_size and page_size are provided in bytes
3. all the numerical inputs provided in the beginning must be greater than zero.
4. try to give all the commands in a legitimate order.
