/**************************************************************
 *
 * userprog/ksyscall.h
 *
 * Kernel interface for systemcalls
 *
 * by Marcus Voelp  (c) Universitaet Karlsruhe
 *
 **************************************************************/

#ifndef __USERPROG_KSYSCALL_H__ 
#define __USERPROG_KSYSCALL_H__ 

#include "kernel.h"
#include "synchconsole.h"
#include <time.h>
#include <stdlib.h>
#include <stdint.h>

#define MAX_LENGTH_INT 11


void SysHalt()
{
  kernel->interrupt->Halt();
}


int SysAdd(int op1, int op2)
{
  return op1 + op2;
}

/**/

int SysReadNum()
{
  char numChar[MAX_LENGTH_INT + 1];
  memset(numChar, 0, sizeof(numChar));
  char temp = kernel->synchConsoleIn->GetChar();
  int index = 0;

  if (isdigit(temp) || temp == '-') {}
  else
    {
      DEBUG(dbgSys, "Weird..." << '\n');
      DEBUG(dbgSys, "temp: " << (int) temp << '\n');
      return -1;
    }
  
  while (temp != (char)10)
  {
    DEBUG(dbgSys, "temp: " << (int) temp << '\n');
    numChar[index++] = temp;
    if (index > MAX_LENGTH_INT)
    {
      if (numChar[0] == '-')
      {
        DEBUG(dbgSys, "Invalid input - invalid length" << '\n'); 
        return INT32_MIN;
      }
      else
      {
        DEBUG(dbgSys, "Invalid input - invalid length" << '\n'); 
        return INT32_MAX;
      }
    }
    temp = kernel->synchConsoleIn->GetChar();
  }

  int num = 0;
  sscanf(numChar, "%d", &num);
  return num;
}

/**/

void SysPrintNum(int num)
{
  char numChar[MAX_LENGTH_INT + 1];
  sprintf(numChar, "%d", num);
  for (unsigned int i = 0; i < strlen(numChar); i++)
    kernel->synchConsoleOut->PutChar(numChar[i]);
  kernel->synchConsoleOut->PutChar('\n');
}

/**/
int SysRandomNum()
{
  srandom(time(NULL));
  return random();
}

/**/
char SysReadChar()
{
  return kernel->synchConsoleIn->GetChar();
}

/**/
void SysPrintChar(char c)
{
  kernel->synchConsoleOut->PutChar(c);
}

#endif /* ! __USERPROG_KSYSCALL_H__ */
