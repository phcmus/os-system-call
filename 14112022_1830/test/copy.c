#include "syscall.h"
#define MAX_LENGTH 32

int main() {
    char dst[MAX_LENGTH + 1], src[MAX_LENGTH + 1];
    char* c;
    OpenFileId dstId, srcId;
    int size = 0, i = 0;

    PrintString("Input source file: ");
    ReadString(src, MAX_LENGTH + 1);
    
    if ((srcId = Open(src, 0)) == -1)
    {
        PrintString("Can not open file\n");
        return 0;
    }
    PrintString("Source opened successfully\n");

    PrintString("Input destination file: ");
    ReadString(dst, MAX_LENGTH + 1);
    
    PrintString("Creating file...\n");
    Create(dst);
    dstId = Open(dst, 1);
    PrintString("Destination file created successfully\n");

    size = Seek(-1, srcId);
    i = Seek(0, srcId);

    Seek(-1, dstId);

    while (i < size)
    {
        Read(c, 1, srcId);
        Write(c, 1, dstId);
        i++;
    }

    Close(dstId);
    Close(srcId);
    return 0;
}