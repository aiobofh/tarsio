#ifndef _OPTIONS_H_
#define _OPTIONS_H_

#include <stdlib.h>

struct options_s {
  char* code_filename;
  char* testcases_filename;
  int cache;
  int test;
  int header;
};
typedef struct options_s options_t;

#define OPTIONS_EMPTY {NULL, NULL, 0, 0, 0}

int options_init(options_t* options, int argc, char* argv[]);
void options_cleanup(options_t* options);

#endif
