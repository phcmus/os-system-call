#include "syscall.h"
#define MAX_LENGTH 32

int main() {
    char dst[MAX_LENGTH + 1], src[MAX_LENGTH + 1];
    char* c;
    OpenFileId dstId, srcId;
    int size = 0, newSize = 0, i = 0;

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
    
    if ((dstId = Open(dst, 0)) == -1)
    {
        PrintString("Can not open file\n");
        return 0;
    }
    PrintString("Destination opened successfully\n");

    size = Seek(-1, dstId);
    Seek(0, dstId);

    i = Seek(-1, srcId);

    newSize = i + size;

    while (i < newSize)
    {
        Read(c, 1, dstId);
        Write(c, 1, srcId);
        i++;
    }

    Close(dstId);
    Close(srcId);
    return 0;
}