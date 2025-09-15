#include <assert.h>
// #include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>

#define MAX_LINE 1024
// Functions

/*
 * handleCmdLineArgs -
 *
 * @
 * @
 *
 * Returns:
 * */
FILE *handleCmdLineArgs(int argc, char *argv[]);

char **readLineByLine(FILE *file);
char **parse_line(char *buffer);

// Free strings allocated for arguments
void free_args(char **args);

int main(int argc, char *argv[]) {

  FILE *file = handleCmdLineArgs(argc, argv);

  if (!file)
    return EXIT_FAILURE;

  char line[MAX_LINE];

  while (fgets(line, MAX_LINE, file)) {

    char **args = parse_line(line);
    if (!args)
      exit(EXIT_FAILURE);

    if (args[0] == NULL) {
      free_args(args);
      continue;
    }

    int pid = fork();

    if (pid == -1) {
      perror("fork");
      exit(EXIT_FAILURE);
    } else if (pid == 0) {
      execvp(args[0], args);
      perror("execvp");
      exit(EXIT_FAILURE);
    } else {
      int status;
      waitpid(pid, &status, 0);
      if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
        exit(EXIT_FAILURE);
    }

    free_args(args);
  }

  return EXIT_SUCCESS;
}

FILE *handleCmdLineArgs(int argc, char *argv[]) {
  if (argc > 2) {
    fprintf(stderr, "Usage: %s [file]\n", argv[0]);

    return NULL;
  }

  if (argc == 2) {
    FILE *file = fopen(argv[1], "r");

    if (!file) {
      perror("fopen");
      return NULL;
    }

    return file;
  }

  return stdin;
}

char **parse_line(char *buffer) {
  int max_args = 30;
  char **args = malloc(sizeof(char *) * max_args);

  if (!args) {
    fprintf(stderr, "Couldnt allocate memory for arguments");
    return NULL;
  }

  char *tokens = strtok(buffer, " \t\n");

  int i = 0;
  while (tokens != NULL) {
    args[i] = strdup(tokens);
    i++;
    tokens = strtok(NULL, " \t\n");
  }

  args[i] = NULL;
  return args;
}

void free_args(char **args) {
  if (args == NULL)
    return;
  for (int i = 0; args[i] != NULL; i++) {
    free(args[i]);
  }
  free(args);
}
