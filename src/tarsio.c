#include <stdlib.h>
#include <string.h>

#include "tarsio.h"

void __tarsio_handle_arguments(int argc, char* argv[]) {
  (void)argc;
  (void)argv;
}

void __tarsio_unit_test_execute(__tarsio_data_t* __tarsio_mock_data, int (*func)(void* __tarsio_mock_data), const char* name, size_t mock_data_size) {
  (void)name;
  memset(__tarsio_mock_data, 0, mock_data_size);
  func(__tarsio_mock_data);
}
