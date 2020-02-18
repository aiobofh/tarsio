#include <stdio.h>

#include "write_file.h"

static int write_file(const char* output_filename, void* data, size_t size) {
  const FILE* fd = fopen(output_filename, "w+");

  if (NULL == fd) {
    return WRITE_FILE_FOPEN_FAILED;
  }

  const size_t result = fwrite(data, 1, size, (FILE*)fd);
  const int fwrite_ok = (size == result);

  if (0 != fclose((FILE*)fd)) {
    return WRITE_FILE_FCLOSE_FAILED;
  }

  if (!fwrite_ok) {
    return WRITE_FILE_FWRITE_FAILED;
  }

  return WRITE_FILE_EVERYTHING_IS_OK;
}

/* A function that make use of write_file() just to remove warnings */
int main(int argc, char* argv[]) {
  (void)argc;
  (void)argv;
  unsigned char data[4] = {0, 1, 2, 3};
  write_file("/tmp/foo.txt", data, sizeof(data));
  return 0;
}
