#include <assert.h>
// #include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

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
