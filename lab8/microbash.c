#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
 // char* writer_args[] = {"./writer", NULL};
 // char* reader_args[] = {"./reader", NULL};
  
  for (int i = 0; i < argc; i++) {
    printf("%s\n", argv[i]);
  }
  char* writer_args[] = {argv[2], NULL};
  char* reader_args[] = {argv[1], NULL};
  char* new_args[] = {argv[3], NULL};
 
  int fdsA[2];
  int fdsB[2];

  pipe(fdsA);
  pipe(fdsB);

  // First child
  if (fork() == 0) {
    dup2(fdsA[1], 1);
    close(fdsA[0]);
    close(fdsB[0]);
    close(fdsB[1]);
    if(execv(writer_args[0], writer_args) == -1)
      exit(1);
  }
  // Second child
  if (fork() == 0) {
    dup2(fdsA[0], 0);
    dup2(fdsB[1], 1);
    close(fdsA[1]);
    close(fdsB[0]);
    if(execv(reader_args[0], reader_args) == -1)
      exit(1);
  }
  
  if (fork() == 0) {
    dup2(fdsB[0], 0);
    close(fdsA[0]);
    close(fdsA[1]);
    close(fdsB[1]);
    if(execv(new_args[0], reader_args) == -1)
      exit(1);
  }

  close(fdsA[0]);
  close(fdsA[1]);
  close(fdsB[0]);
  close(fdsB[1]);

  // Parent
  int status;
  wait(&status);
  wait(&status);
}

