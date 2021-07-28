// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	are in machine.h.
//----------------------------------------------------------------------

void IncreasePC()
{
	int counter = machine->ReadRegister(PCReg);
	machine->WriteRegister(PrevPCReg, counter);
	counter = machine->ReadRegister(NextPCReg);
	machine->WriteRegister(PCReg, counter);
	machine->WriteRegister(NextPCReg, counter +4);
}

char* User2Sys(int virtualAddr, int limit)
{
	int oneChar;
	char* kernelBuffer = NULL;
	kernelBuffer = new char[limit+1];
	if (kernelBuffer == NULL) return kernelBuffer;
	memset(kernelBuffer,0,limit+1);
	for (int i = 0; i < limit; i++)
	{
		machine->ReadMem(virtualAddr + i, 1, &oneChar);
		kernelBuffer[i] = (char) oneChar;
		if (oneChar == 0) break;
	}
	return kernelBuffer;
}

int Sys2User(int virtualAddr, int len, char*buffer)
{
	if (len <0) return -1;
	if (len ==0) return len;
	int i = 0;
	int oneChar = 0;
	do
	{
		oneChar = (int)buffer[i];
		machine->WriteMem(virtualAddr + i, 1, oneChar);
		i++;
	} while (i< len && oneChar !=0);
	return i;
}

void ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);

    switch (which) {
	case NoException:
	return;

        case PageFaultException:
		{
		printf("\n\n No valid translation found");
		interrupt->Halt();
		break;
		}

        case ReadOnlyException:
		{
		printf("\n\n Write attempted to page marked read-only");
		interrupt->Halt();
		break;
		}

        case BusErrorException:
		{
		printf("\n\n Translation resulted in an invalid physical address");
		interrupt->Halt();
		break;
		}

        case AddressErrorException:
		{
		printf("\n\n Unaligned reference or one that was beyond the end of the address space");
		interrupt->Halt();
		break;
		}

        case OverflowException:
		{
		printf("\n\n Integer overflow in add or sub");
		interrupt->Halt();
		break;
		}

	case IllegalInstrException:
		{
		printf("\n\n Unimplemented or reserved instr");
		interrupt->Halt();
		break;	
		}

	case SyscallException:
	switch (type) {
		case SC_Halt:
		{	
			printf("\nShutdown, initiated by user program. \n");
			interrupt->Halt();
			return;
		}
		
		case SC_ReadInt:
		{
		    // Doc so nguyen tu man hinh console.
                    char* buffer;
                    int MAX_BUFFER = 255;
                    buffer = new char[MAX_BUFFER + 1];
                    int numbytes = gSynchConsole->Read(buffer, MAX_BUFFER);// doc buffer toi da MAX_BUFFER ki tu, sau do tra ve so ki tu doc dc
                    int number = 0; // ket qua cuoi cung doc duoc
						
                    // Qua trinh chuyen doi tu buffer sang so nguyen int
			
                    // Xac dinh so am hay so duong                       
                    bool isNegative = false; // Gia thiet so nhap vao la so duong.
                    int firstNumIndex = 0;
                    int lastNumIndex = 0;
                    if(buffer[0] == '-')
                    {
                        isNegative = true;
                        firstNumIndex = 1;
                        lastNumIndex = 1;                        			   		
                    }
                    
                    // Kiem tra tinh hop le cua so nguyen 
                    for(int i = firstNumIndex; i < numbytes; i++)					
                    {
                        if(buffer[i] == '.') // 15.0 van la so nguyen
                        {
                            int j = i + 1;
                            for(; j < numbytes; j++)
                            {
				// Truong hop so khong hop le
                                if(buffer[j] != '0')
                                {
                                    printf(" The integer number is not valid");
                                    DEBUG('a', "\n The integer number is not valid");
                                    machine->WriteRegister(2, 0);
                                    IncreasePC();
                                    delete buffer;
                                    return;
                                }
                            }
                            
                            lastNumIndex = i - 1;				
                            break;                           
                        } // neu khong co dau cham
                        else if(buffer[i] < '0' && buffer[i] > '9')
                        {
                            printf("\n\n The integer number is not valid");
                            DEBUG('a', "\n The integer number is not valid");
                            machine->WriteRegister(2, 0);
                            IncreasePC();
                            delete buffer;
                            return;
                        }
                        lastNumIndex = i;    
                    }			
                    
                    // La so nguyen hop le, tien hanh chuyen chuoi ve so nguyen
                    for(int i = firstNumIndex; i<= lastNumIndex; i++)
                    {
                        number = number * 10 + (int)(buffer[i] - 48); 
                    }
                    
                    // neu la so am thi * -1;
                    if(isNegative)
                    {
                        number = number * -1;
                    }
                    machine->WriteRegister(2, number);
                    IncreasePC();
                    delete buffer;
                    return;		
		}

		case SC_PrintInt:
		{	
		    // In so nguyen len man hinh console
                    int number = machine->ReadRegister(4);
		    if(number == 0)
                    {
                        gSynchConsole->Write("0", 1); // In ra man hinh so 0
                        IncreasePC();
                        return;    
                    }
                    
                    /*Qua trinh chuyen so thanh chuoi de in ra man hinh*/
                    bool isNegative = false; // gia su la so duong
                    int numberOfNum = 0; // Bien de luu so chu so cua number
                    int firstNumIndex = 0; 
			
                    if(number < 0)
                    {
                        isNegative = true;
                        number = number * -1; // Chuyen so am thanh so duong de tinh so chu so
                        firstNumIndex = 1; 
                    } 	
                    
                    int t_number = number; // bien tam cho number
                    while(t_number)
                    {
                        numberOfNum++;
                        t_number /= 10;
                    }
    
		    // Tao mang dem chuoi de in ra man hinh
                    char* buffer;
                    int MAX_BUFFER = 255;
                    buffer = new char[MAX_BUFFER + 1];
		    // chuyen so thanh ky tu
                    for(int i = firstNumIndex + numberOfNum - 1; i >= firstNumIndex; i--)
                    {
                        buffer[i] = (char)((number % 10) + 48);
                        number /= 10;
                    }
                    if(isNegative) // neu la so am, them dau '-' o dang truoc
                    {
                        buffer[0] = '-';
			buffer[numberOfNum + 1] = 0;
                        gSynchConsole->Write(buffer, numberOfNum + 1);
                        delete buffer;
                        IncreasePC();
                        return;
                    }
		    buffer[numberOfNum] = 0;	
                    gSynchConsole->Write(buffer, numberOfNum);
                    delete buffer;
                    IncreasePC();
                    return;        			
					
		}

		case SC_ReadString:
		{	
			int virtualAddr, length;
			char* buffer;
			virtualAddr = machine->ReadRegister(4);
			length = machine->ReadRegister(5);
			buffer = User2Sys(virtualAddr, length);
			gSynchConsole->Read(buffer, length);
			Sys2User(virtualAddr, length, buffer);
			delete buffer;
			IncreasePC();
			return;
		}

		case SC_PrintString:
		{	
			int virtualAddr;
			char* buffer;
			virtualAddr = machine->ReadRegister(4);
			buffer = User2Sys(virtualAddr, 255);
			int length = 0;
			while (buffer[length] != 0) length++;
			gSynchConsole->Write(buffer, length + 1);
			delete buffer;
			IncreasePC();
			return;
		}

		case SC_ReadChar:
		{	
			//Input: 1 ky tu 
			//Output: Khong co
			//Doc vao 1 ky tu kieu char
			int maxBytes = 255;
			char* buffer = new char[255];
			int numBytes = gSynchConsole->Read(buffer, maxBytes);

			if(numBytes >1) //Nhap nhieu hon 1 ky tu thi bao loi
			{
				printf("Input only a character ");
				machine->WriteRegister(2, 0);
			}
			else if(numBytes ==0) //NULL
			{
				printf("NULL!");
				machine->WriteRegister(2, 0);
			}
			else
			{
				//Dung 1 ky tu, lay ky tu o i = 0 roi dua vao r2
				char c = buffer[0];
				machine->WriteRegister(2, c);
			}
			delete buffer;
			IncreasePC();
			return;
		}
			
		case SC_PrintChar:
		{
			//Input: Ki tu loai char
			//Output: Ki tu loai char
			//Xuat ki tu ra man hinh
			char c = (char)machine->ReadRegister(4);
			gSynchConsole->Write(&c, 1);
			IncreasePC();
			return;
		}
	}
	default: 
		break;
}
	IncreasePC();	
}
		
