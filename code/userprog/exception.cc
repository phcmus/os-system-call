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
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "debug.h"
#include "ksyscall.h"
#include "main.h"
#include "synchconsole.h"
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
// If you are handling a system call, don't forget to increment the pc
// before returning. (Or else you'll loop making the same system call forever!)
//
//	"which" is the kind of exception.  The list of possible exceptions
//	is in machine.h.
//----------------------------------------------------------------------

char *User2System(int virtAddr, int limit) {
  int i; // chi so index
  int oneChar;
  char *kernelBuf = NULL;
  kernelBuf = new char[limit + 1]; // can cho chuoi terminal
  if (kernelBuf == NULL)
    return kernelBuf;

  memset(kernelBuf, 0, limit + 1);

  for (i = 0; i < limit; i++) {
    kernel->machine->ReadMem(virtAddr + i, 1, &oneChar);
    kernelBuf[i] = (char)oneChar;
    if (oneChar == 0)
      break;
  }
  return kernelBuf;
}

// Input: Khong gian vung nho User(int) - gioi han cua buffer(int) - bo nho dem
// buffer(char*) Output: So byte da sao chep(int) Chuc nang: Sao chep vung nho
// System sang vung nho User
int System2User(int virtAddr, int len, char *buffer) {
  if (len < 0)
    return -1;
  if (len == 0)
    return len;
  int i = 0;
  int oneChar = 0;
  do {
    oneChar = (int)buffer[i];
    kernel->machine->WriteMem(virtAddr + i, 1, oneChar);
    i++;
  } while (i < len && oneChar != 0);
  return i;
}

void IncreasePC() {

  DEBUG(dbgSys, "IncreasePC");

  kernel->machine->WriteRegister(PrevPCReg,
                                 kernel->machine->ReadRegister(PCReg));

  /* set programm counter to next instruction (all Instructions are 4 byte
   * wide)*/
  kernel->machine->WriteRegister(PCReg,
                                 kernel->machine->ReadRegister(PCReg) + 4);

  /* set next programm counter for brach execution */
  kernel->machine->WriteRegister(NextPCReg,
                                 kernel->machine->ReadRegister(PCReg) + 4);
}

void ExceptionHandler(ExceptionType which) {
  int type = kernel->machine->ReadRegister(2);

  DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");

  switch (which) {
  case SyscallException:
    switch (type) {
    case SC_Halt:
      DEBUG(dbgSys, "Shutdown, initiated by user program.\n");

      SysHalt();

      ASSERTNOTREACHED();
      break;

    case SC_Add:
      DEBUG(dbgSys, "Add " << kernel->machine->ReadRegister(4) << " + "
                           << kernel->machine->ReadRegister(5) << "\n");

      /* Process SysAdd Systemcall*/
      int result;
      result = SysAdd(/* int op1 */ (int)kernel->machine->ReadRegister(4),
                      /* int op2 */ (int)kernel->machine->ReadRegister(5));

      DEBUG(dbgSys, "Add returning with " << result << "\n");
      /* Prepare Result */
      kernel->machine->WriteRegister(2, (int)result);

      /* Modify return point */
      {
        /* set previous programm counter (debugging only)*/
        kernel->machine->WriteRegister(PrevPCReg,
                                       kernel->machine->ReadRegister(PCReg));

        /* set programm counter to next instruction (all Instructions are 4 byte
         * wide)*/
        kernel->machine->WriteRegister(
            PCReg, kernel->machine->ReadRegister(PCReg) + 4);

        /* set next programm counter for brach execution */
        kernel->machine->WriteRegister(
            NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
      }

      return;

      ASSERTNOTREACHED();

      break;

    case SC_PrintString: {

      DEBUG(dbgSys, "Print string, initiated by user program.\n");

      int virtAddr;
      char *buffer;
      virtAddr = kernel->machine->ReadRegister(4);
      buffer = User2System(virtAddr, 255);
      // Copy chuoi tu vung nho User Space sang
      // System Space voi bo dem buffer dai 255 ki tu
      int length = 0;
      while (buffer[length] != 0)
        kernel->synchConsoleOut->PutChar(buffer[length++]);

      delete buffer;

      kernel->machine->WriteRegister(2, 0);

      break;
    }
    case SC_ReadString: {

      DEBUG(dbgSys, "Read string, initiated by user program.\n");

      int virtAddr, length;
      char *buffer;
      virtAddr = kernel->machine->ReadRegister(4);
      length = kernel->machine->ReadRegister(5);
      buffer = User2System(virtAddr, length);

      DEBUG(dbgSys, virtAddr);
      int tmp = -1;
      do {
        buffer[++tmp] = kernel->synchConsoleIn->GetChar();
        DEBUG(dbgSys, (int)(buffer[tmp] - '\0'));
      } while (buffer[tmp] != '\0' && buffer[tmp] != (char)10);

      System2User(virtAddr, length, buffer);

      delete buffer;

      kernel->machine->WriteRegister(2, 0);

      break;
    }

    case SC_ReadChar: {
      char result = SysReadChar();
      DEBUG(dbgSys, "ReadChar returning with " << result << "\n");
      kernel->machine->WriteRegister(2, result);
      break;
    }
    case SC_PrintChar: {
      char result = (char)kernel->machine->ReadRegister(4);
      DEBUG(dbgSys, "PrintChar returning with " << result << "\n");
      SysPrintChar(result);
      kernel->machine->WriteRegister(2, result);
      break;
    }

    case SC_ReadNum: {
      int result = SysReadNum();
      DEBUG(dbgSys, "ReadNum returning with " << result << "\n");
      kernel->machine->WriteRegister(2, result);
      break;
    }

    case SC_PrintNum: {
      int output = kernel->machine->ReadRegister(4);
      SysPrintNum(output);
      break;
    }

    case SC_RandomNum: {
      int result = SysRandomNum();
      DEBUG(dbgSys, "RandomNum returning with " << result << "\n");
      kernel->machine->WriteRegister(2, result);
      break;
    }

    default:
      cerr << "Unexpected system call " << type << "\n";
      break;
    }
    break;
  default:
    cerr << "Unexpected user mode exception" << (int)which << "\n";
    break;
  }
  IncreasePC();
}
