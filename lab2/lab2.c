#include <stdio.h>
#include <stdlib.h>

#define NUM_BYTES 8

int* get_nums();
void print_vals(void* addr);
void print_ints(int* arr, int count);


int main()
{
  int* vals = get_nums();
  //print_ints(vals, 2);

  char* char_vals = (char*) vals; // convert pointer to 8 byte type
  void* addr = &char_vals[1];     // get teh addrress one byte in and change pointer to 4 byte type 
  int* i = (int*)addr;       // 
  i[0] = 0;





  print_vals(vals);
  return 0;
}


/*
  Returns an array of ints
*/
int* get_nums()
{
  int arr[2];
  // pretend these numbers are loaded out of a file or read from the keyboard
  // we'll hard-code them to make it easier
  arr[0] = -1;
  arr[1] = 28537;
  //int* retval = arr;
  int* retval = (int*)malloc(sizeof(int) * 2);
  retval[0] = arr[0];
  retval[1] = arr[1];
  return retval;
}


void print_ints(int* arr, int count)
{
  printf("printing %d ints starting at %p\n", count, arr);
  for(int i = 0; i < count; i++)
    printf("%d\n", arr[i]);
  printf("\n");
}


void print_vals(void* addr)
{
  int count = 0;
  
  // 1. treat the bytes as ints
  int* iptr = (int*)addr;
  count = NUM_BYTES / sizeof(int);
  printf("printing as ints\n");
  for(int i = 0; i < count; i++)
    printf("(%p): %d\n", &iptr[i], iptr[i]);
  printf("\n");

  // 2. treat the bytes as unsigned ints
  
  unsigned int* uptr = (unsigned int*)addr;
  count = NUM_BYTES / sizeof(unsigned int);
  printf("printing as unsigned ints\n");
  for(int i = 0; i < count; i++)
    printf("(%p): %u\n", &uptr[i], uptr[i]);
  printf("\n");
  

  
  // 3. treat the bytes as hex values
  
  count = NUM_BYTES / sizeof(unsigned int);
  printf("printing as hex\n");
  for(int i = 0; i < count; i++)
    printf("(%p): 0x%08x\n", &uptr[i], uptr[i]);
  printf("\n");
  

  
  // 4. treat the bytes as unsigned char (an 8-bit number)
  
  unsigned char* cptr = (unsigned char*)addr;
  count = NUM_BYTES / sizeof(unsigned char);
  printf("printing as unsigned char\n");
  for(int i = 0; i < count; i++)
    printf("(%p): %3u\t%02x\n", &cptr[i], cptr[i], cptr[i]);
  printf("\n");
  

  
  // 5. treat the bytes as long ints
  
  long int* lptr = (long int*)addr;
  count = NUM_BYTES / sizeof(long int);
  printf("printing as long ints\n");
  for(int i = 0; i < count; i++)
    printf("(%p): %ld\n", &lptr[i], lptr[i]);
  printf("\n");
  

  
  // 6. treat the bytes as a string
  
  char* str = (char*)addr;
  printf("printing as a string\n");
  printf("%s\n", str);

  
}

