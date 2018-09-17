#include <stdlib.h>
#include <string.h>

#include "tarsio.h"

struct __tarsio_stats_s {
  size_t success;
  size_t fail;
  size_t error;
  size_t skip;
};
typedef struct __tarsio_stats_s __tarsio_stats_t;

struct __tarsio_options_s {
  int verbose;
  int xml_output;
};
typedef struct __tarsio_options_s __tarsio_options_t;

__tarsio_options_t __tarsio_options;
__tarsio_stats_t __tarsio_stats;

void __tarsio_assert_eq(int res, const char* testcase_name) {
  if (res) {
    __tarsio_stats.fail++;
  }
  else {
    __tarsio_stats.success++;
  }
}

void __tarsio_init(void) {
  memset(&__tarsio_stats, 0, sizeof(__tarsio_stats));
}

void __tarsio_handle_arguments(int argc, char* argv[]) {
  (void)argc;
  (void)argv;
}

void __tarsio_skip(const char* reason, const char* test_name) {
  __tarsio_stats.skip++;
}

void __tarsio_unit_test_execute(__tarsio_data_t* __tarsio_mock_data, int (*func)(void* __tarsio_mock_data, const char* __tarsio_test_name), const char* name, size_t mock_data_size) {
  size_t skip = __tarsio_stats.skip;
  size_t fail = __tarsio_stats.fail;
  size_t error = __tarsio_stats.error;
  memset(__tarsio_mock_data, 0, mock_data_size);
  func(__tarsio_mock_data, name);
  if (1 == __tarsio_options.verbose) {
    if (skip != __tarsio_stats.skip) {
      printf("[SKIP] %s\n", name);
    }
    else if (error != __tarsio_stats.error) {
      printf("[ERROR] %s\n", name);
    }
    else if (fail != __tarsio_stats.fail) {
      printf("[FAIL] %s\n", name);
    }
    else {
      printf("[PASS] %s\n", name);
    }
  }
  else {
    if (skip != __tarsio_stats.skip) {
      putc('S', stdout);
    }
    else if (error != __tarsio_stats.error) {
      putc('E', stdout);
    }
    else if (fail != __tarsio_stats.fail) {
      putc('F', stdout);
    }
    else {
      putc('.', stdout);
    }
  }
}

void __tarsio_summary(void) {
  putc('\n', stdout);
}
