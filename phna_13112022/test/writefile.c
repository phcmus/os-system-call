#include "syscall.h"
#define MAX_LENGTH 32

int main() {
    int openFileId;
    int fileSize;
    char c; // Ky tu de in ra
    char fileName[MAX_LENGTH];
    int i; // Index for loop

    if ((openFileId = Open("test.txt", 0)) == -1) // Goi ham Open de mo file
    {
        PrintString("cannot open file");
    }
    PrintString("OpenFileId ");
    PrintNum(openFileId);
    if (openFileId != 0) // Kiem tra Open co loi khong
    {
        Write("Something new", 13, openFileId);
        Close(openFileId); // Goi ham Close de dong file
    } else {
        PrintString(" -> Mo file khong thanh cong!!\n\n");
    }

    return 1;
}
