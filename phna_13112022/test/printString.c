/* printString.c
 *	Simple program to test whether the systemcall interface works.
 */

#include "syscall.h"

int main() {
  PrintString("\32 Operating system 20_22\n");
  // Halt();
  /* not reached */
}
