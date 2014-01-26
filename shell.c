#include "shell.h"
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include "tshield/tshield.h"

#define MAX_ARGS 5
#define IN_LENGTH 80

static int process_command(int argc, char **argv);

void shell() {
  char in[IN_LENGTH];
  int argc;
  char *argv[MAX_ARGS];

  while(1) {
    printf ("root@arduino$ ");

    fgets(in, IN_LENGTH, stdin);

    argc = 0;
    char *p = strtok (in," \n\r");
    while (p != NULL && argc < MAX_ARGS) {
      argv[argc] = p;
      argc += 1;
      p = strtok(NULL, " \n\r");
    }
    printf("%d\n", process_command(argc, argv));
  }
}

static int process_command(int argc, char **argv) {
  if (argc < 1) return -1;

  if (strcmp(argv[0], "led") == 0) {
    return 0;
  }

  return -1;
}