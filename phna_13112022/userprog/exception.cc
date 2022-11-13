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

#define MaxFileLength 32 // Do dai quy uoc cho file name

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
// buffer(char*)
// Output: So byte da sao chep(int)
// Chuc nang: Sao chep vung nho System sang vung nho User
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
            result =
                SysAdd(/* int op1 */ (int)kernel->machine->ReadRegister(4),
                       /* int op2 */ (int)kernel->machine->ReadRegister(5));

            DEBUG(dbgSys, "Add returning with " << result << "\n");
            /* Prepare Result */
            kernel->machine->WriteRegister(2, (int)result);

            /* Modify return point */
            {
                /* set previous programm counter (debugging only)*/
                kernel->machine->WriteRegister(
                    PrevPCReg, kernel->machine->ReadRegister(PCReg));

                /* set programm counter to next instruction (all Instructions
                 * are 4 byte wide)*/
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
            // read buffer address from register 4
            virtAddr = kernel->machine->ReadRegister(4);
            // convert memory from user space to system
            // space in order to system can read
            int length = 0;
            buffer = User2System(virtAddr, 255);
            while (buffer[length] != 0)
                // print current character to console
                kernel->synchConsoleOut->PutChar(buffer[length++]);

            delete buffer; // free memory, prevent memory leak

            // return status code (0: success) to register 2
            kernel->machine->WriteRegister(2, 0);

            break;
        }
        case SC_ReadString: {

            DEBUG(dbgSys, "Read string, initiated by user program.\n");

            int virtAddr, length;
            char *buffer;
            // read buffer address from register 4
            virtAddr = kernel->machine->ReadRegister(4);
            // read max length value from register 5
            length = kernel->machine->ReadRegister(5);
            // convert memory from user space to system
            // space in order to system can read
            buffer = User2System(virtAddr, length);

            DEBUG(dbgSys, virtAddr);
            int tmp = -1;
            do {
                buffer[++tmp] = kernel->synchConsoleIn->GetChar();
                // get character from buffer

                DEBUG(dbgSys, (int)(buffer[tmp] - '\0'));
            } while (buffer[tmp] != '\0' && buffer[tmp] != (char)10);
            // read new character until

            // convert memory back to user space to return string read
            System2User(virtAddr, length, buffer);

            delete buffer; // free memory, prevent memory leak

            // return status code (0: success) to register 2
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

        case SC_CreateFile: {
            // Input: Dia chi tu vung nho user cua ten file
            // Output: -1 = Loi, 0 = Thanh cong
            // Chuc nang: Tao ra file voi tham so la ten file
            int virtAddr;
            char *filename;
            DEBUG('a', "\n SC_CreateFile call ...");
            DEBUG('a', "\n Reading virtual address of filename");

            virtAddr = kernel->machine->ReadRegister(
                4); // Doc dia chi cua file tu thanh ghi R4
            DEBUG('a', "\n Reading filename.");

            // Sao chep khong gian bo nho User sang System, voi do dang toi da
            // la (32 + 1) bytes
            filename = User2System(virtAddr, MaxFileLength + 1);
            if (strlen(filename) == 0) {
                printf("\n File name is not valid");
                DEBUG('a', "\n File name is not valid");
                kernel->machine->WriteRegister(
                    2, -1); // Return -1 vao thanh ghi R2
                // IncreasePC();
                // return;
                break;
            }

            if (filename == NULL) // Neu khong doc duoc
            {
                printf("\n Not enough memory in system");
                DEBUG('a', "\n Not enough memory in system");
                kernel->machine->WriteRegister(
                    2, -1); // Return -1 vao thanh ghi R2
                delete filename;
                // IncreasePC();
                // return;
                break;
            }
            DEBUG('a', "\n Finish reading filename.");

            // Tao file bang ham Create cua
            // fileSystem, tra ve ket qua
            if (!kernel->fileSystem->Create(filename, 0)) {
                // Tao file that bai
                printf("\n Error create file '%s'", filename);
                kernel->machine->WriteRegister(2, -1);
                delete filename;
                // IncreasePC();
                // return;
                break;
            }

            DEBUG('a', "\n Tao file thanh cong.");
            // Tao file thanh cong
            kernel->machine->WriteRegister(2, 0);
            delete filename;
            // IncreasePC(); //Day thanh ghi lui ve sau de tiep tuc ghi
            // return;
            break;
        }
        case SC_Open: {
            int vAddr = kernel->machine->ReadRegister(4);
            int type = kernel->machine->ReadRegister(5);
            char *filename;
            filename = User2System(vAddr, MaxFileLength);

            int slot = kernel->fileSystem->FindFreeSlot();
            if (slot != -1) {
                if (type == 0 || type == 1) {
                    if ((kernel->fileSystem->openf[slot] =
                             kernel->fileSystem->Open(filename, type)) !=
                        NULL) {
                        kernel->machine->WriteRegister(2, slot);
                    } else {
                        kernel->machine->WriteRegister(2, -1);
                        delete[] filename;
                        break;
                    }
                } else if (type == 2)
                    kernel->machine->WriteRegister(2, 0);
                else if (type == 3)
                    kernel->machine->WriteRegister(2, 1);
                else
                    kernel->machine->WriteRegister(2, -1);
                delete[] filename;
                break;
            }

            // Case SC_Open fail, return -1
            kernel->machine->WriteRegister(2, -1);
            delete[] filename;
            break;
        }

        case SC_Close: {
            int id = kernel->machine->ReadRegister(4);
            if (id >= 0 && id <= 9) {
                if (kernel->fileSystem->openf[id]) {
                    delete kernel->fileSystem->openf[id];
                    kernel->fileSystem->openf[id] = NULL;
                    kernel->machine->WriteRegister(2, 0);
                    break;
                }
            }

            // Case SC_Close fail, return -1
            kernel->machine->WriteRegister(2, -1);
            break;
        }

        case SC_Seek: {
            int pos = kernel->machine->ReadRegister(4);
            int file_id = kernel->machine->ReadRegister(5);

            if (file_id < 2 || file_id > kernel->fileSystem->index ||
                kernel->fileSystem->openf[file_id] == NULL) {
                kernel->machine->WriteRegister(2, -1);
                break;
            }

            // Get filesize
            int len = kernel->fileSystem->openf[file_id]->Length();

            // Check if pos is larger than filesize
            // or pos points to end of file
            if (pos > len) {
                kernel->machine->WriteRegister(2, -1);
                break;
            }

            if (pos == -1)
                pos = len;

            // Seek exec
            kernel->fileSystem->openf[file_id]->Seek(pos);
            kernel->machine->WriteRegister(2, pos);
            break;
        }

        case SC_Read: {
            // Input: buffer(char*), so ky tu(int), id cua file(OpenFileID)
            // Output: -1: Loi, So byte read thuc su: Thanh cong, -2: Thanh cong
            // Cong dung: Doc file voi tham so la buffer, so ky tu cho phep va
            // id cua file
            int virtAddr = kernel->machine->ReadRegister(
                4); // Lay dia chi cua tham so buffer tu thanh ghi so 4
            int charcount = kernel->machine->ReadRegister(
                5); // Lay charcount tu thanh ghi so 5
            int id = kernel->machine->ReadRegister(
                6); // Lay id cua file tu thanh ghi so 6
            int OldPos;
            int NewPos;
            char *buf;
            // Kiem tra id cua file truyen vao co nam ngoai bang mo ta file
            // khong
            if (id < 0 || id > 14) {
                printf("\nKhong the read vi id nam ngoai bang mo ta file.");
                kernel->machine->WriteRegister(2, -1);
                IncreasePC();
                return;
            }
            // Kiem tra file co ton tai khong
            if (kernel->fileSystem->openf[id] == NULL) {
                printf("\nKhong the read vi file nay khong ton tai.");
                kernel->machine->WriteRegister(2, -1);
                IncreasePC();
                return;
            }
            if (kernel->fileSystem->openf[id]->type ==
                3) // Xet truong hop doc file stdout (type quy uoc la 3) thi tra
                   // ve -1
            {
                printf("\nKhong the read file stdout.");
                kernel->machine->WriteRegister(2, -1);
                IncreasePC();
                return;
            }
            OldPos = kernel->fileSystem->openf[id]
                         ->GetCurrentPos(); // Kiem tra thanh cong
                                            // thi lay vi tri OldPos
            buf = User2System(
                virtAddr,
                charcount); // Copy chuoi tu vung nho User Space sang System
                            // Space voi bo dem buffer dai charcount
            // Xet truong hop doc file stdin (type quy uoc la 2)
            if (kernel->fileSystem->openf[id]->type == 2) {
                // Su dung ham Read cua lop SynchConsole de tra ve so byte thuc
                // su doc duoc
                int size = -1;
                do {
                    buf[++size] = kernel->synchConsoleIn->GetChar();
                    // get character from buf

                    DEBUG(dbgSys, (int)(buf[size] - '\0'));
                } while (buf[size] != '\0' && buf[size] != (char)10);
                System2User(
                    virtAddr, size,
                    buf); // Copy chuoi tu vung nho System Space sang User Space
                          // voi bo dem buffer co do dai la so byte thuc su
                kernel->machine->WriteRegister(
                    2,
                    size); // Tra ve so byte thuc su doc duoc
                delete buf;
                IncreasePC();
                return;
            }
            // Xet truong hop doc file binh thuong thi tra ve so byte thuc su
            if ((kernel->fileSystem->openf[id]->Read(buf, charcount)) > 0) {
                // So byte thuc su = NewPos - OldPos
                NewPos = kernel->fileSystem->openf[id]->GetCurrentPos();
                // Copy chuoi tu vung nho System Space sang User Space voi bo
                // dem buffer co do dai la so byte thuc su
                System2User(virtAddr, NewPos - OldPos, buf);
                kernel->machine->WriteRegister(2, NewPos - OldPos);
            } else {
                // Truong hop con lai la doc file co noi dung la NULL tra ve -2
                // printf("\nDoc file rong.");
                kernel->machine->WriteRegister(2, -2);
            }
            delete buf;
            IncreasePC();
            return;
        }

        case SC_Write: {
            // Input: buffer(char*), so ky tu(int), id cua file(OpenFileID)
            // Output: -1: Loi, So byte write thuc su: Thanh cong, -2: Thanh
            // cong Cong dung: Ghi file voi tham so la buffer, so ky tu cho phep
            // va id cua file
            int virtAddr = kernel->machine->ReadRegister(
                4); // Lay dia chi cua tham so buffer tu thanh ghi so 4
            int charcount = kernel->machine->ReadRegister(
                5); // Lay charcount tu thanh ghi so 5
            int id = kernel->machine->ReadRegister(
                6); // Lay id cua file tu thanh ghi so 6
            int OldPos;
            int NewPos;
            char *buf;
            // Kiem tra id cua file truyen vao co nam ngoai bang mo ta file
            // khong
            if (id < 0 || id > 14) {
                printf("\nKhong the write vi id nam ngoai bang mo ta file.");
                kernel->machine->WriteRegister(2, -1);
                IncreasePC();
                return;
            }
            // Kiem tra file co ton tai khong
            if (kernel->fileSystem->openf[id] == NULL) {
                printf("\nKhong the write vi file nay khong ton tai.");
                kernel->machine->WriteRegister(2, -1);
                IncreasePC();
                return;
            }
            // Xet truong hop ghi file only read (type quy uoc la 1) hoac file
            // stdin (type quy uoc la 2) thi tra ve -1
            if (kernel->fileSystem->openf[id]->type == 1 ||
                kernel->fileSystem->openf[id]->type == 2) {
                printf("\nKhong the write file stdin hoac file only read.");
                kernel->machine->WriteRegister(2, -1);
                IncreasePC();
                return;
            }
            OldPos = kernel->fileSystem->openf[id]
                         ->GetCurrentPos(); // Kiem tra thanh cong
                                            // thi lay vi tri OldPos
            buf = User2System(
                virtAddr,
                charcount); // Copy chuoi tu vung nho User Space sang System
                            // Space voi bo dem buffer dai charcount
            // Xet truong hop ghi file read & write (type quy uoc la 0) thi tra
            // ve so byte thuc su
            if (kernel->fileSystem->openf[id]->type == 0) {
                if ((kernel->fileSystem->openf[id]->Write(buf, charcount)) >
                    0) {
                    // So byte thuc su = NewPos - OldPos
                    NewPos = kernel->fileSystem->openf[id]->GetCurrentPos();
                    kernel->machine->WriteRegister(2, NewPos - OldPos);
                    delete buf;
                    IncreasePC();
                    return;
                }
            }
            if (kernel->fileSystem->openf[id]->type ==
                3) // Xet truong hop con lai ghi file stdout (type quy uoc la 3)
            {
                int i = 0;
                while (buf[i] != 0 &&
                       buf[i] !=
                           '\n') // Vong lap de write den khi gap ky tu '\n'
                {
                    kernel->synchConsoleOut->PutChar(buf[i++]);
                }
                buf[i] = '\n';
                kernel->synchConsoleOut->PutChar(buf[i]);
                kernel->machine->WriteRegister(
                    2, i - 1); // Tra ve so byte thuc su write duoc
                delete buf;
                IncreasePC();
                return;
            }
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
