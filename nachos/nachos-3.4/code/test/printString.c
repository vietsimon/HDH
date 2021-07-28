#include "syscall.h"
#include "copyright.h"

int main()
{
	char str[200];
	PrintString("input number");
	ReadString(str, 200);
	PrintString(str);
	PrintString("\n");
	Halt();
	return 0;
}
