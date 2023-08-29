#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

#define MAX_LEN 50

void many_sleep();

/*
 * Read a command from the keyboard and return whether or not it was
 * requested as a background process (&)
*/
int read_command(char* command[2])
{
  size_t size = MAX_LEN;
  getline(&(command[0]), &size, stdin);

  // Remove the newline
  command[0][strlen(command[0])-1] = '\0';

  command[1] = NULL;
  
  if(command[0][strlen(command[0])-1] == '&')
  {
    // remove the space and ampersand from the command so we can execute it
    command[0][strlen(command[0])-2] = '\0';
    return 1;
  }
  return 0;
}

// The main program and the sigchld_handler will use this 
// global variable to communicate whether or not there is a foreground process
// to wait for.
static volatile int fg_pid = 0;


/* 
 * Take action when SIGCHLD arrives
*/
void sigchld_handler(int signum)
{
  int status;
  int finished_pid;

  // We must reap potentially multiple children because signals are not queued.
  // We use WNOHANG so we can stop this loop as soon as no zombies are available.
  while((finished_pid = waitpid(-1, &status, WNOHANG)) > 0)
  {
    // "notify" the busy loop in main that it can stop waiting for the foreground process
    if(finished_pid == fg_pid)
      fg_pid = 0;
  }

}


int main(int argc, char** argv)
{
  int status;
  pid_t pid;
  char* command[2];
  command[0] = (char*)malloc(MAX_LEN);
  char* env[1] = {NULL};
  sigset_t blocked;
  sigset_t empty;
  sigemptyset(&empty);
  
 
  // install a handler for SIGCHLD
  signal(SIGCHLD, sigchld_handler);
  
  while(1)
  {
    printf("[tiny shell]$ ");
    int is_bg = read_command(command);
    
    // Two built-in shell commands "quit" and "many_sleep"
    if(!strcmp(command[0], "quit"))
      break;

    if(!strcmp(command[0], "many_sleep"))
    {
      many_sleep();
      continue;
    }

    // Block SIGCHLD while we determine if there is a foreground process to wait for
    // This avoids a race with the sigchld_handler
    sigemptyset(&blocked);
    sigaddset(&blocked, SIGCHLD);
    sigprocmask(SIG_BLOCK, &blocked, NULL);

    pid = fork();

    if(pid == 0) // if child
    {
      if(execve(command[0], command, env) == -1)
      {
	printf("unknown command: %s\n", command[0]);
	exit(1);
      }
    }
    
    if(!is_bg)
      fg_pid = pid;
    
    // After setting fg_pid, we can unblock SIGCHLD
    sigprocmask(SIG_UNBLOCK, &blocked, NULL);

    // Wait for the foreground process
    while(fg_pid != 0)
    {
      // instead of a busy loop, use sigsuspend as discussed in class
      // but be careful not to keep signals blocked for background processes
    }

  }

}


/*
 * This built-in command simulates running many background commands simultaneously in the shell
*/
void many_sleep()
{
  int i;
  int pid;

  char* command[2] = {"./sleep5", NULL};
  char* env[1] = {NULL};
  

  for(i = 0; i < 30; i++)
  {
    pid = fork();

    if(pid == 0) // if child
    {
      if(execve(command[0], command, env) == -1)
      {
	printf("error executing sleep5\n");
	exit(1);
      }
    }
    
    // Nothing for the parent to do since the children are background processes
  }
}
