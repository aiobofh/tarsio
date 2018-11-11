#include <stdlib.h>
#include <string.h>

#include "error.h"

#include "str.h"

char* strclone(const char* src) {
  char *dst;

  if (NULL == (dst = malloc(strlen(src) + 1))) {
    error1("Out of memory while allocating string clone '%s'", src);
    goto malloc_failed;
  }

  if (NULL == strcpy(dst, src)) {
    error1("Could not copy string clone '%s'", src);
    goto strcpy_failed;
  }

  goto normal_exit;

 strcpy_failed:
  free(dst);
  dst = NULL;
 malloc_failed:
 normal_exit:
  return dst;
}
