#include "syscall.h"
#define MAX_LENGTH 32

int main() {
    int openFileId;
    int fileSize;
    char c; // Ky tu de in ra
    char fileName[MAX_LENGTH];
    int i; // Index for loop
    // PrintString("\n---TEST CAT---\n\n");
    // PrintString("\n\t\t\t-----HIEN THI NOI DUNG FILE-----\n\n");
    // PrintString(" - Nhap vao ten file can doc: ");

    // Goi ham ReadString de doc vao ten file
    // Co the su dung Open(stdin), nhung de tiet kiem thoi gian test ta dung
    // ReadString

    // PrintString(fileName);

    if ((openFileId = Open("test.txt", 1)) == -1) // Goi ham Open de mo file
    {
        PrintString("cannot open file");
    }
    PrintString("OpenFileId ");
    PrintNum(openFileId);
    if (openFileId != 0) // Kiem tra Open co loi khong
    {
        // Seek den cuoi file de lay duoc do dai noi dung (fileSize)
        fileSize = Seek(0, openFileId);
        PrintNum(fileSize);
        i = 1;
        // Seek den dau tap tin de tien hanh Read
        Seek(1, openFileId);

        PrintString(" -> Noi dung file:\n");
        for (; i < fileSize; i++) // Cho vong lap chay tu 1 - fileSize
        {
            Read(&c, 2,
                 openFileId); // Goi ham Read de doc tung ki tu noi dung file
            PrintChar(c);     // Goi ham PrintChar de in tung ki tu ra man hinh
        }
        Close(openFileId); // Goi ham Close de dong file
    } else {
        PrintString(" -> Mo file khong thanh cong!!\n\n");
    }

    return 1;
}
