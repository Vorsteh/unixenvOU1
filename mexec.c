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

// Free strings & commands allocated for arguments
void free_args(char **args);

int main(int argc, char *argv[]) {

  FILE *file = handleCmdLineArgs(argc, argv);

  if (!file)
    return EXIT_FAILURE;

  char line[MAX_LINE];
  int commands_amount = 0;
  char ***commands = NULL;

  while (fgets(line, MAX_LINE, file)) {

    char **args = parse_line(line);
    if (!args)
      exit(EXIT_FAILURE);

    if (args[0] == NULL) {
      free_args(args);
      continue;
    }

    commands = realloc(commands, sizeof(char **) * (commands_amount + 1));

    if (!commands) {
      perror("realloc");
      exit(EXIT_FAILURE);
    }

    commands[commands_amount] = args;
    commands_amount++;
  }

  int pipes_amount = commands_amount - 1;
  int pipes[pipes_amount][2];
  for (int i = 0; i < pipes_amount; i++) {
    if (pipe(pipes[i]) == -1) {
      perror("pipe");
      exit(EXIT_FAILURE);
    }
  }

  for (int i = 0; i < commands_amount; i++) {
    int pid = fork();
    if (pid == -1) {
      perror("fork");
      exit(EXIT_FAILURE);
    }

    if (pid == 0) {
      if (i > 0)
        dup2(pipes[i - 1][0], STDIN_FILENO);
      if (i < commands_amount - 1)
        dup2(pipes[i][1], STDOUT_FILENO);

      for (int j = 0; j < pipes_amount; j++) {
        close(pipes[j][0]);
        close(pipes[j][1]);
      }

      execvp(commands[i][0], commands[i]);
      perror("execvp");
      exit(EXIT_FAILURE);
    }
  }

  for (int i = 0; i < pipes_amount; i++) {
    close(pipes[i][0]);
    close(pipes[i][1]);
  }

  int status;
  for (int i = 0; i < commands_amount; i++) {
    wait(&status);
    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
      exit(EXIT_FAILURE);
    }
  }

  // CLEANUP

  if (file != stdin) {
    fclose(file);
  }
  for (int i = 0; i < commands_amount; i++) {
    free_args(commands[i]);
  }
  free(commands);

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
