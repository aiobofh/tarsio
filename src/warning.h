#ifndef _WARNING_H_
#define _WARNING_H_

#include <stdio.h>

#define warning0(STR) fprintf(stderr, "WARNING: " STR "\n");
#define warning1(FMT, ARG1) fprintf(stderr, "WARNING: " FMT "\n", ARG1);

#endif
