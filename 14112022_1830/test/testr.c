#include "syscall.h"

int main()
{
    OpenFileId fileId;
    char filename[33];

    PrintString("Input file name: ");
    ReadString(filename, 33);

    if ((fileId = Open("testr.txt", 1)) == -1)
    {
        PrintString("Can not open file\n");
        Close(fileId);
        return 0;
    }

    PrintNum(fileId);
    PrintChar('\n');
    PrintString("File test.txt opened successfully\n");
    
    PrintString("Delete file...\n");
    Remove(filename);
    // if (Remove(filename) == -2)
    //     PrintString("Can not delete file, file is opened\n");
    
    Close(fileId);
    return 0;
}