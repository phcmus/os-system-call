#include "syscall.h"
#define MAX_LENGTH 32

int main()
{
    char filename[MAX_LENGTH + 1];

    PrintString("Input file name to delete: ");
    ReadString(filename, MAX_LENGTH + 1);
    
    PrintString("Delete file...\n");
    Remove(filename);
    return 0;
}