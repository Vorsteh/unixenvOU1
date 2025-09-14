#include <assert.h>
// #include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
void free_args(char **args) {
  if (args == NULL)
    return;
  for (int i = 0; args[i] != NULL; i++) {
    free(args[i]);
  }
  free(args);
}

int main(int argc, char *argv[]) {

  FILE *file = handleCmdLineArgs(argc, argv);

  if (!file)
    return EXIT_FAILURE;

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
      perror("Couldnt open the file");
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
