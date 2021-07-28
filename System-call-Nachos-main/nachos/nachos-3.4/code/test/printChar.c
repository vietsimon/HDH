#include "syscall.h"

void main()
{
	char c;
	c=ReadChar();
	PrintChar(c);
	Halt();
}
