#include <string.h>

#include "error.h"

#include "options.h"

static int get_long_opt(const char* str, options_t* options, int cnt) {
  int i;
  for (i = 0; i < cnt; i++) {
    if (0 == strcmp(str, options[i].long_opt)) {
      options[i].enabled = 1;
      return 1;
    }
  }
  return 0;
}

static int get_short_opt(const char* str, options_t* options, int cnt) {
  int i;
  for (i = 0; i < cnt; i++) {
    if (str[0] == options[i].short_opt) {
      options[i].enabled = 1;
      return 1;
    }
  }
  return 0;
}

static int get_other_opt(const char* str, options_t* options, int cnt) {
  int i;
  for (i = 0; i < cnt; i++) {
    if (0 == strcmp(str, options[i].other_opt)) {
      options[i].enabled = 1;
      return 1;
    }
  }
  return 0;
}

int options_init(int argc, char* argv[], options_t* options, int cnt) {
  int i;
  int retval = 0;
  for (i = 1; i < argc; i++) {
    if ('-' == argv[i][0]) { /* UNIX style */
      if ('-' == argv[i][1]) {
        if (0 == get_long_opt(&argv[i][2], options, cnt)) {
          error1("Unknown long-opt '%s'", argv[i]);
          return -1;
        }
        retval++;
      }
      if (0 == get_short_opt(&argv[i][1], options, cnt)) {
        error1("Unknown short-opt '%s'", argv[i]);
        return -1;
      }
      retval++;
    }
    else if ((('A' <= argv[i][0]) && ('Z' >= argv[i][0])) || ('?' == argv[i][0])) {
      if (0 == get_other_opt(&argv[i][2], options, cnt)) {
        error1("Unknown option '%s'", argv[i]);
        return -1;
      }
      retval++;
    }
  }
  return retval;
}
