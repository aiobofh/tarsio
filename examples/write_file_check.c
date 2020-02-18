#include <tarsio.h>

#include "write_file.h"
#include "write_file_data.h"

test(shall_return_WRITE_FILE_EVERYTHING_IS_OK_if_everything_is_ok) {
  tarsio_mock.fopen.retval = (FILE*)0x1234;
  tarsio_mock.fwrite.retval = 10;
  assert_eq(WRITE_FILE_EVERYTHING_IS_OK, write_file("some_file_path.dat", (void*)0x5678, 10));
}

test(shall_close_the_correct_file_if_opened) {
  tarsio_mock.fopen.retval = (FILE*)0x1234;
  write_file("some_file_path.dat", (void*)0x5678, 10);
  assert_eq(1, tarsio_mock.fclose.call_count);
  assert_eq((FILE*)0x1234, tarsio_mock.fclose.args.arg0);
}

test(shall_not_close_a_file_by_accided_if_file_was_not_opened) {
  write_file("some_file_path.dat", NULL, 0);
  assert_eq(0, tarsio_mock.fclose.call_count);
}

test(return_WRITE_FILE_FOPEN_FAILED_if_fopen_failes) {
  assert_eq(WRITE_FILE_FOPEN_FAILED, write_file("some_file_path.dat", NULL, 0));
}

test(write_the_data_to_the_correct_file) {
  tarsio_mock.fopen.retval = (FILE*)0x1234;
  write_file("some_file_path.dat", (void*)0x5678, 10);
  assert_eq(1, tarsio_mock.fwrite.call_count);
  assert_eq((void*)0x5678, tarsio_mock.fwrite.args.arg0);
  assert_eq(10, tarsio_mock.fwrite.args.arg1);
  assert_eq(1, tarsio_mock.fwrite.args.arg2);
  assert_eq((FILE*)0x1234, tarsio_mock.fwrite.args.arg3);
}

test(return_WRITE_FILE_FWRITE_FAILED_if_file_write_fails) {
  tarsio_mock.fopen.retval = (FILE*)0x1234;
  assert_eq(WRITE_FILE_FWRITE_FAILED, write_file("some_file_path.dat", (void*)0x5678, 10));
}

test(return_WRITE_FILE_FCLOSE_FAILED_if_file_could_not_be_closed) {
  tarsio_mock.fopen.retval = (FILE*)0x1234;
  tarsio_mock.fclose.retval = EOF;
  assert_eq(WRITE_FILE_FCLOSE_FAILED, write_file("some_file_path.dat", (void*)0x5678, 10));
}
