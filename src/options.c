#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "options.h"

int options_init(options_t* options, int argc, char* argv[]) {
  assert((NULL != options) && "Argument 'options' must not be NULL");
  /*
  int i = 0;
  for (i = 0; i < argc; i++) {
    if ((0 == strcmp(argv[i], "-c")) || (0 == strcmp(argv[i], "--cache"))) {
      options->cache = 1;
    }
    else if ((0 == strcmp(argv[i], "-t")) || (0 == strcmp(argv[i], "--test"))) {
      options->test = 1;
    }
    else if ((0 == strcmp(argv[i], "-h")) || (0 == strcmp(argv[i], "--header"))) {
      options->header = 1;
    }
  }

  if (options->cache) {

  }
  */
  if ((3 != argc) && (1 != argc)) {
    fprintf(stderr, "ERROR: Illegal number of arguments.\n");
    return -1;
  }
  options->code_filename = argv[1];
  options->testcases_filename = argv[2];
  return 0;
}

void options_cleanup(options_t* options) {
  assert((NULL != options) && "Argument 'options' must not be NULL");
  *options = (options_t)OPTIONS_EMPTY;
}
