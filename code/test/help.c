/* help.c
 *	Simple program to test whether the systemcall interface works.
 */

#include "syscall.h"

int main() {
  PrintString("Thành viên nhóm:\n");
  PrintString("\t1. Phạm Hoàng Nam Anh. - 18120278.\n");
  PrintString("\t2. Nguyễn Linh Đăng Dương. - 20120274\n\n");
  PrintString(
      "Chương trình ASCII: Dùng một biến char chứa kí tự chuẩn bịin và vòng "
      "lặp để in các giá trị từ 32-126 (các kí tự có thể hiển thị).\n\n");
  PrintString(
      "Chương trình sort: Dùng system call ReadNum cho người "
      "dùng nhập vào mảng số. Dùng thuật toán Bubble Sort để sắp xếp mảng số "
      "này và dùng system call PrintNum để xuất ra mảng số đã sắp xếp.\n\n");
}
