#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>

void
changeDir(char *input) {
  // checks if chdir fails
  if(chdir(input) != 0) {
    perror("chdir() to desired path failed");
    exit(-1);
  }
}

void
getHostname(char *host) {
  // get the hostname properly for Linux
  if (gethostname(host, sizeof(host)) != 0) { // success = 0, failure = -1
    perror("Host name couldn't be found");
    exit(-1);
  }
}

void
strealloc(char **args, char *string, int i)
{
    size_t n = strlen(string) + 1;
    args[i] = (char *) realloc(args[i], n);
    if (args[i])
      memcpy(args[i], string, n);
    else {
      perror("realloc failed");
      exit(1);
    }
}

int
parseString(char *string, char **args, int *lastPointer) {
  char *token;
  int i = 0;

  // initializes the tokenization
  token = strtok(string, " \n");
    // until the last character
    while(token != NULL)
    {
      strealloc(args, token, i);
      i++;
      token = strtok(NULL, " \n");
    }
  args[i] = NULL;
  if(i > *lastPointer) return i;
  else return *lastPointer;
}

int
readCommand(char *host, char **args, int *lastPointer) {
  char pwd[100], input[100];
  
  // prints the hostname and computername
  printf("%s@%s:%s> ", getenv("USER"), host, getcwd(pwd, sizeof(pwd)));
  // gets the input string
  fgets(input, sizeof(input), stdin);
  // calls the parser function
  return parseString(input, args, lastPointer);
}

void
createChild(char **args) {
  pid_t rc = fork();
    
  if(rc < 0) {
    perror("fork failed");
    exit(1);
  } else if(rc == 0) {
    if(execvp(args[0], args) == -1) {
      perror("exec failed");
      exit(1);
    }
  } else {
    wait(&rc);
  }
}

int
main(int argc, char *argv[])
{
  char p[100], host[100];
  char *args[100] = {NULL};
  int lastPointer = 0;

  if(argc >= 3) {
    // checks if initialization is correct
    fprintf(stderr, "usage: ./sshell <path>");
    exit(-1);
  } else if(argc == 2) {
    // initializes with a pwd
    strcpy(p, argv[1]);
  } else {
    // gets the path inserted
    printf("Insert the desired path working directory: ");
    scanf("%s", p);
    getchar();
  }

  changeDir(p);
  getHostname(host);
  
  do {
    // save the value of last pointer allocated
    lastPointer = readCommand(host, args, &lastPointer);
    // stop condition
    if(strcmp(args[0], "exit") == 0 || strcmp(args[0], "quit") == 0) break;
    // calls for a new child process and executes the command
    createChild(args);
    // exit commands
  } while(1);

  // freeing allocated memory used for strealloc
  for(int j = 0; j <= lastPointer; j++) {
    free(args[j]);
  }
  return 0;
}