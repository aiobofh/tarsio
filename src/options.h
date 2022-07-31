#ifndef _OPTIONS_H_
#define _OPTIONS_H_

struct options_s {
  char short_opt;
  const char* long_opt;
  const char* other_opt;
  const char* arg;
  int enabled;
};
typedef struct options_s options_t;

int options_init(int argc, char* argv[], options_t* options, int cnt);
#endif
