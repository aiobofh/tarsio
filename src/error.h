#ifndef _ERROR_H_
#define _ERROR_H_

#include <stdio.h>

/*
#define error0(STR)
#define error1(FMT, ARG1)
#define error2(FMT, ARG1, ARG2)
#define error3(FMT, ARG1, ARG2, ARG3)
#define error4(FMT, ARG1, ARG2, ARG3, ARG4)
*/

#define error0(STR) fprintf(stderr, "%s:%d ERROR: " STR "\n", __FILE__, __LINE__)
#define error1(FMT, ARG1) fprintf(stderr, "%s:%d ERROR: " FMT "\n", __FILE__, __LINE__, ARG1)
#define error2(FMT, ARG1, ARG2) fprintf(stderr, "%s:%d ERROR: " FMT "\n", __FILE__, __LINE__, ARG1, ARG2)
#define error3(FMT, ARG1, ARG2, ARG3) fprintf(stderr, "%s:%d ERROR: " FMT "\n", __FILE__, __LINE__, ARG1, ARG2, ARG3)
#define error4(FMT, ARG1, ARG2, ARG3, ARG4) fprintf(stderr, "%s:%d ERROR: " FMT "\n", __FILE__, __LINE__, ARG1, ARG2, ARG3, ARG4)

#endif
