/* readString.c
 *	Simple program to test whether the systemcall interface works.
 */

#include "syscall.h"
#define MAX_LENGTH 32

int main() {
  char bufnr[MAX_LENGTH];
  PrintString("Input your string:");
  ReadString(bufnr, MAX_LENGTH);
  PrintString("Your string is: ");
  PrintString(bufnr);
}
