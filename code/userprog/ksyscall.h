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

// Do dai toi da cua mot INTEGER (tinh ca dau '-')
#define MAX_LENGTH_INT 11


void SysHalt()
{
  kernel->interrupt->Halt();
}


int SysAdd(int op1, int op2)
{
  return op1 + op2;
}

// Ham ho tro SysCall ReadNum
int SysReadNum()
{
  // Khoi tao mang ki tu de chua input cua nguoi dung
  char numChar[MAX_LENGTH_INT + 1];
  memset(numChar, 0, sizeof(numChar));
  
  // Doc ki tu dau tien, xac dinh input la am hay duong
  // index: Chi so de them vao mang ki tu numChar
  char temp = kernel->synchConsoleIn->GetChar();
  int index = 0;

  // Kiem tra neu ki tu dau tien la ki tu khong hop le
  if (isdigit(temp) || temp == '-') {}
  else
    {
      DEBUG(dbgSys, "First character invalid" << '\n');
      DEBUG(dbgSys, "temp: " << (int) temp << '\n');
      return -1;
    }
  
  // Bat dau doc input den khi gap ki tu ' '
  while (temp != (char)10)
  {
    DEBUG(dbgSys, "temp: " << (int) temp << '\n');
    // Them ki tu vao mang 
    // Dong nay duoc dat tai day vi temp da doc mot ki tu truoc khi vao vong lap
    numChar[index++] = temp;
    
    // Xu li truong hop do dai chuoi nhap vao khong hop le
    // Tra ve so INTEGER lon nhat hoac nho nhat
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
    
    // Kiem tra ki tu vua doc duoc co hop le hay khong
    if (isdigit(temp) || temp == '-') {}
    else
    {
      DEBUG(dbgSys, "Invalid character" << '\n');
      DEBUG(dbgSys, "temp: " << (int) temp << '\n');
      return -1;
    }
    
    // Doc ki tu moi
    temp = kernel->synchConsoleIn->GetChar();
  }
  
  // Chuyen doi mang ki tu numChar, luu vao mot so INTEGER
  // negative: Kiem tra so am, 1 neu ki tu dau la '-', 0 neu nguoc lai
  // num: bien INTEGER luu ket qua
  bool negative = (numChar[0] == '-' ? 1 : 0);
  int num = 0;
  for (int i = negative; i < index; i++)
    num += (numChar[i] - 48) * pow(10, index - 1 - i);
  
  // Tra ve ket qua
  if (negative)
    return -num;
  return num;
}

// Ham ho tro SysCall PrintNum
void SysPrintNum(int num)
{
  DEBUG(dbgSys, "INTEGER RECEIVED: " << num << '\n');
  
  // Khoi tao mang de luu cac ki tu chu so cua num (so can in ra)
  char numChar[MAX_LENGTH_INT + 2];
  memset(numChar, 0, sizeof(numChar));
  
  // temp: Copy gia tri cua num, thuc hien xu ly chu yeu tren temp
  int temp = 0;
  if (num == INT32_MIN)
    temp = INT32_MAX;
  else
    temp = abs(num);
  
  DEBUG(dbgSys, "TEMP: " << temp << '\n');
  // index: chi so de them vao mang numChar
  int index = 0;
  
  // Thuc hien chuyen doi INTEGER num sang mang ki tu
  // Cac ki tu chu so them vao mang se bi dao lai
  // Ex: 123 -> numChar: 3 2 1
  while (temp > 0)
  {
    int calc = (temp % 10) + 48;
    numChar[index++] = (char) calc;
    temp /= 10;
  }
  
  DEBUG(dbgSys, "INDEX: " << index << '\n');
  // Thuc hien in
  // Neu so am thi in truoc ki tu '-'
  if (num < 0)
  {
    kernel->synchConsoleOut->PutChar('-');
  }

  // INT32_MAX < |INT32_MIN| 1 don vi nen ta cong them 1 don vi vao ki tu dau tien cua mang numChar
  if (num == INT32_MIN)
  {
    numChar[0] += 1;
  }
  
  // In tat ca cac ki tu chu so trong mang numChar (in nguoc)
  for (int i = index - 1; i >= 0; i--)
  {
    kernel->synchConsoleOut->PutChar(numChar[i]);
  }
  return;
}

// Ham ho tro SysCall RandomNum
int SysRandomNum()
{
  // Tao mot seed moi moi mot giay thoi gian thuc
  // Tra ve so random(Seed vua tao)
  srandom(time(NULL));
  return random();
}

// Ham ho tro SysCall ReadChar
char SysReadChar()
{
  return kernel->synchConsoleIn->GetChar();
}

// Ham ho tro SysCall PrintChar
void SysPrintChar(char c)
{
  kernel->synchConsoleOut->PutChar(c);
}

#endif /* ! __USERPROG_KSYSCALL_H__ */
