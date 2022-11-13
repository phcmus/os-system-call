#include "syscall.h"

#define SIZE (100)

int A[SIZE];

void swap(int* a, int* b)
{
  int temp = *a;
  *a = *b;
  *b = temp;
}

int main() 
{
  int n, i, j, c;
  PrintString("Input array size: ");
  n = ReadNum();
  for (i = 0; i < n; i++)
  {
    PrintString("A[");
    PrintNum(i);
    PrintString("]: ");
    A[i] = ReadNum();
  }
  
  PrintString("Array:\n");
  for (i = 0; i < n; i++)
  {
    PrintNum(A[i]);
    PrintChar(' ');
  }
  PrintChar('\n');
  
  PrintString("Choose which sort direction:\n");
  PrintString("1: Ascending, 2: Descending\n");
  PrintString("Input: ");
  c = ReadNum();
  while (c != 1 && c != 2)
  {
    PrintString("Invalid choice\n");
    PrintString("Choose which sort direction:\n");
    PrintString("1: Ascending, 2: Descending\n");
    PrintString("Input: ");
    c = ReadNum();
  }

  if (c == 1)
  {
    for (i = 0; i < n - 1; i++)
      for (j = 0; j < n - i - 1; j++)
        if (A[j] > A[j + 1])
          swap(&A[j], &A[j + 1]);
  }
  else if (c == 2)
  {
    for (i = 0; i < n - 1; i++)
      for (j = 0; j < n - i - 1; j++)
        if (A[j] < A[j + 1])
          swap(&A[j], &A[j + 1]);
  }

  PrintString("Array post sort:\n");
  for (i = 0; i < n; i++)
  {
    PrintNum(A[i]);
    PrintChar(' ');
  }
  PrintChar('\n');
}
