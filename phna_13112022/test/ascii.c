/* ascii.c
 *	Simple program to test whether the systemcall interface works.
 */

#include "syscall.h"

int main() {
  int i;
  for (i = 32; i < 127; i++) {
    char c = (char)i;
    if (i % 8 == 0) {
      PrintChar('\n');
    }
    PrintChar(c);
    PrintChar(' ');
  }
  PrintChar('\n');
  // Halt();
  /* not reached */
}
