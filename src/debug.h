#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <stdio.h>

#ifdef NODEBUG
#define debug0(STR)
#define debug1(FMT, ARG1)
#define debug2(FMT, ARG1, ARG2)
#define debug3(FMT, ARG1, ARG2, ARG3)
#define debug4(FMT, ARG1, ARG2, ARG3, ARG4)
#define debug5(FMT, ARG1, ARG2, ARG3, ARG4, ARG5)
#define debug6(FMT, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6)
#define debug7(FMT, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7)
#else
#define debug0(STR) fprintf(stderr, "DEBUG: " STR "\n")
#define debug1(FMT, ARG1) fprintf(stderr, "DEBUG: " FMT "\n", ARG1)
#define debug2(FMT, ARG1, ARG2) fprintf(stderr, "DEBUG: " FMT "\n", ARG1, ARG2)
#define debug3(FMT, ARG1, ARG2, ARG3) fprintf(stderr, "DEBUG: " FMT "\n", ARG1, ARG2, ARG3)
#define debug4(FMT, ARG1, ARG2, ARG3, ARG4) fprintf(stderr, "DEBUG: " FMT "\n", ARG1, ARG2, ARG3, ARG4)
#define debug5(FMT, ARG1, ARG2, ARG3, ARG4, ARG5) fprintf(stderr, "DEBUG: " FMT "\n", ARG1, ARG2, ARG3, ARG4, ARG5)
#define debug6(FMT, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6) fprintf(stderr, "DEBUG: " FMT "\n", ARG1, ARG2, ARG3, ARG4, ARG5, ARG6)
#define debug7(FMT, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7) fprintf(stderr, "DEBUG: " FMT "\n", ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7)
#endif

#endif
