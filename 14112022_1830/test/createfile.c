#include "syscall.h"
#define MAX_LENGTH 32

int main() {
    char filename[MAX_LENGTH + 1];
    PrintString("Input filename: ");
    ReadString(filename, MAX_LENGTH + 1);
    Create(filename);
    return 0;
}