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
#include <math.h>

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
      DEBUG(dbgSys, "First character invalid" << '\n');
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

    if (isdigit(temp) || temp == '-') {}
    else
    {
      DEBUG(dbgSys, "First character invalid" << '\n');
      DEBUG(dbgSys, "temp: " << (int) temp << '\n');
      return -1;
    }
    temp = kernel->synchConsoleIn->GetChar();
  }

  bool negative = (numChar[0] == '-' ? 1 : 0);
  int num = 0;
  for (int i = negative; i < index; i++)
    num += (numChar[i] - 48) * pow(10, index - 1 - i);
  if (negative)
    return -num;
  return num;
}

/**/


void SysPrintNum(int num)
{
  DEBUG(dbgSys, "INTEGER RECEIVED: " << num << '\n');
  
  char numChar[MAX_LENGTH_INT + 1];
  memset(numChar, 0, sizeof(numChar));
  int temp = abs(num);
  DEBUG(dbgSys, "TEMP: " << temp << '\n');
  int index = 0;

  while (temp > 0)
  {
    int calc = (temp % 10) + 48;
    numChar[index++] = (char) calc;
    temp /= 10;
  }

  DEBUG(dbgSys, "INDEX: " << index << '\n');

  if (num < 0)
  {
    kernel->synchConsoleOut->PutChar('-');
  }

  for (int i = index - 1; i >= 0; i--)
  {
    kernel->synchConsoleOut->PutChar(numChar[i]);
  }
  return;
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
