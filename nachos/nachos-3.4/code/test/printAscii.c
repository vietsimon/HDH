#include "syscall.h"

void main()
{
	int a;
    PrintChar('\n');
    PrintString("-----Chuong trinh in ra bang ma ASCII------\n");
    PrintChar('\n');
    PrintString("BANG MA ASCII\n");
    for (a = 0; a < 127; a++) {
         PrintInt(a);
        PrintChar(':');
       
	PrintChar(a);
        PrintChar('\n');
    }
    Halt();
}

