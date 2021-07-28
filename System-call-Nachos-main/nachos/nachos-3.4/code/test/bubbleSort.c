#include "syscall.h"

void main()
{
	int n,i,j,temp;
	int A[100];
	PrintString("-------Chuong trinh Bubble Sort---------\n");
	
	PrintString("Nhap n:  ");
	n=ReadInt();
	while(n>100){
	PrintString("Yeu cau 1<=n<=100\n");
	PrintString("Nhap lai n:  ");
	n=ReadInt();
	}
	for (i=0;i<n;i++) {
	PrintString("A[");
	PrintInt(i);
	PrintString("]= ");
	A[i]=ReadInt();
	}
	for (i=0;i<n-1;i++) {
	  for (j=n-1;j>i;j--) {
		if (A[j-1]>A[j]) {
	     	    temp=A[j];
		    A[j]=A[j-1];
		    A[j-1]=temp;
		}
	  }
	}
	PrintString("\n ----Day so sau khi duoc sap xep--------\n");
	for (i=0;i<n;i++) {
	    PrintInt(A[i]);
	    PrintChar(' ');
	}
	Halt();
}
