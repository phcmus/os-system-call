#include "syscall.h"
#define MAX_LENGTH 32

int main() {
    char filename[MAX_LENGTH + 1];
    char* c;
    OpenFileId fileId;
    int size = 0, i = 0;
    PrintString("Input filename to print content: ");
    ReadString(filename, MAX_LENGTH + 1);
    
    if ((fileId = Open(filename, 0)) == -1)
    {
        PrintString("Can not open file\n");
        return 0;
    }

    PrintString("File opened successfully\n");

    size = Seek(-1, fileId);
    i = Seek(0, fileId);

    while (i < size)
    {
        Read(c, 1, fileId);
        PrintString(c);
        i++;
    }

    Close(fileId);
    return 0;
}