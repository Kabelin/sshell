#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>

int
main(int argc, char *argv[])
{

  char pwd[100], s[100], host[100], input[100];
  char *token, *args[50];
  int i;
  pid_t rc;

  if(argc >= 3) {
    // checks if initialization is correct
    fprintf(stderr, "usage: ./sshell <path>");
    exit(-1);
  } else if(argc == 2) {
    // initializes with a pwd
    strcpy(s, argv[1]);
  } else {
    // gets the pwd inserted
    printf("Insert the desired path working directory: ");
    scanf("%s", s);
    getchar();
  }
  
  // checks if chdir fails
  if(chdir(s) != 0) {
    perror("chdir() to desired path failed");
    exit(-1);
  }
  
  while(1) {
    i = 0;
    // get the hostname properly for Linux
    if (gethostname(host, sizeof(host)) != 0) { // success = 0, failure = -1
      perror("Host name couldn't be found");
      exit(-1);
    }

    printf("%s@%s:%s> ", getenv("USER"), host, getcwd(pwd, sizeof(pwd)));
    fgets(input, sizeof(input), stdin);
    token = strtok(input, " \n");

    if(strcmp(token, "cd") == 0) {
      i = 2;
      args[0] = strdup(token);
      while(token != NULL)
      {
        if(token != NULL) args[1] = strcat(args[1], token);
        printf("%s\n", args[1]);
        token = strtok(NULL, "\n");
      }
    }
    else {
      while(token != NULL)
      {
        args[i] = strdup(token);
        i++;
        token = strtok(NULL, " \n");
      }
    }
    args[i] = NULL;

    if(strcmp(args[0], "exit") == 0 || strcmp(args[0], "quit") == 0) exit(1);
    
    rc = fork();
    
    if(rc < 0) {
      perror("fork failed");
      exit(1);
    } else if(rc == 0) {
      if(strcmp(args[0], "cd") == 0) {
        printf("desired path: %s\n", args[1]);
        if(chdir(args[1]) != 0) {
          perror("chdir() to desired path failed");
          exit(-1);
        }
      }
      else if(execvp(args[0], args) == -1) {
        perror("exec failed");
        exit(1);
      }
    } 
    wait(&rc);
  }
  
  for(int j = 0; args[j] != NULL; j++) 
  {
    free(args[j]);
  }

  return 0;
}