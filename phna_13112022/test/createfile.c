#include "syscall.h"
#define MAX_LENGTH 32

int main() {
    // int stdin;
    // char fileName[MAX_LENGTH];
    // PrintString("\n\t\t\t-----TAO FILE-----\n\n");
    // PrintString(" - Nhap ten file: ");

    // ReadString(fileName, MAX_LENGTH); // Goi ham Read doc ten file vua nhap

    // if (CreateFile(fileName) == 0) // Goi ham CreateFile
    // {
    //     PrintString(" -> Create file thanh cong.\n\n");
    // }
    CreateFile("test.txt");
    return 0;
}
