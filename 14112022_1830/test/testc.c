#include "syscall.h"

int main()
{
    OpenFileId fileId;
    char filename[33];
    int num = 0;

    PrintString("Input file name: ");
    ReadString(filename, 33);
    if ((fileId = Open(filename, 1)) == -1)
    {
        PrintString("Can not open file\n");
        Close(fileId);
        return 0;
    }
    PrintNum(fileId);
    PrintChar('\n');
    PrintString("File opened successfully\n");
    num = Seek(-1, fileId);
    PrintNum(num);
    PrintChar('\n');
    Close(fileId);
    return 0;
}