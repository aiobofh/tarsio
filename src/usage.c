#include <stdio.h>

#include "usage.h"

void usage(const char* program_name) {
  fprintf(stdout, "USAGE: %s <pre-processed-source> <pre-processed-tests>\n", program_name);
}
