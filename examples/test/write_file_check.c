/*
 * Check-suite for write_file.c
 *
 *              _______          _____ ___        ______
 *                 |      ||    |         |    | |      |
 *                 |      ||    |         |    | |      |
 *                 |   ___||___ |         |___ | |______|
 *
 *              Copyright (C) 2020 AiO Secure Teletronics
 *
 *  This file is part of Tarsio.
 *
 *  Tarsio is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Tarsio is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Tarsio.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <tarsio.h>

#include "write_file.h"
#include "write_file_data.h"

/***************************************************************************
 * Helper functions for the module checks where real files are written to
 * disc, and these are used to read them back to be validated for correct
 * contents.
 */
static size_t fsize(FILE *fd) {
  size_t file_size;
  (void)fseek(fd, 0L, SEEK_END);
  file_size = ftell(fd);
  rewind(fd);
  return file_size;
}

static char* read_file(const char* filename) {
  FILE* fd = fopen(filename, "rb");
  const size_t size = fsize(fd);
  char* retval = malloc(size);
  fread(retval, 1, size, fd);
  fclose(fd);
  return retval;
}

/***************************************************************************
 * Unit-checks
 */
check(shall_open_the_correct_file_for_writing) {
  write_file("some_file_path.dat", (void*)0x5678, 10);
  assert_eq(1, tarsio_mock.fopen.call_count);
  assert_eq(0, strcmp("some_file_path.dat", tarsio_mock.fopen.args.arg0));
  assert_eq(0, strcmp("w", tarsio_mock.fopen.args.arg1));
}

check(shall_return_WRITE_FILE_EVERYTHING_IS_OK_if_everything_is_ok) {
  tarsio_mock.fopen.retval = (FILE*)0x1234;
  tarsio_mock.fwrite.retval = 10;
  assert_eq(WRITE_FILE_EVERYTHING_IS_OK, write_file("some_file_path.dat", (void*)0x5678, 10));
}

check(shall_close_the_correct_file_if_opened) {
  tarsio_mock.fopen.retval = (FILE*)0x1234;
  write_file("some_file_path.dat", (void*)0x5678, 10);
  assert_eq(1, tarsio_mock.fclose.call_count);
  assert_eq((FILE*)0x1234, tarsio_mock.fclose.args.arg0);
}

check(write_file_should_not_write_data_if_fopen_failed) {
  write_file("some_file_path.dat", NULL, 0);
  assert_eq(0, tarsio_mock.fwrite.call_count);
}

check(shall_not_close_a_file_by_accided_if_file_was_not_opened) {
  write_file("some_file_path.dat", NULL, 0);
  assert_eq(0, tarsio_mock.fclose.call_count);
}

check(return_WRITE_FILE_FOPEN_FAILED_if_fopen_failes) {
  assert_eq(WRITE_FILE_FOPEN_FAILED, write_file("some_file_path.dat", NULL, 0));
}

check(write_the_data_to_the_correct_file) {
  tarsio_mock.fopen.retval = (FILE*)0x1234;
  write_file("some_file_path.dat", (void*)0x5678, 10);
  assert_eq(1, tarsio_mock.fwrite.call_count);
  assert_eq((void*)0x5678, tarsio_mock.fwrite.args.arg0);
  assert_eq(1, tarsio_mock.fwrite.args.arg1);
  assert_eq(10, tarsio_mock.fwrite.args.arg2);
  assert_eq((FILE*)0x1234, tarsio_mock.fwrite.args.arg3);
}

check(return_WRITE_FILE_FWRITE_FAILED_if_file_write_fails) {
  tarsio_mock.fopen.retval = (FILE*)0x1234;
  assert_eq(WRITE_FILE_FWRITE_FAILED, write_file("some_file_path.dat", (void*)0x5678, 10));
}

check(return_WRITE_FILE_FCLOSE_FAILED_if_file_could_not_be_closed) {
  tarsio_mock.fopen.retval = (FILE*)0x1234;
  tarsio_mock.fclose.retval = EOF;
  assert_eq(WRITE_FILE_FCLOSE_FAILED, write_file("some_file_path.dat", (void*)0x5678, 10));
}

/***************************************************************************
 * Module-checks
 *
 * Beware! These checks does actually write files to /tmp
 */
module_check(write_file_should_successfully_write_data_to_disk) {
  char* data = "0123456789";
  assert_eq(0, write_file("/tmp/foo.dat", data, strlen(data)));
  assert_eq(0, strcmp(tarsio_mock.fopen.args.arg0, "/tmp/foo.dat"));
  char* result = read_file("/tmp/foo.dat");
  assert_eq(0, strcmp(data, result));
  free(result);
  unlink("/tmp/foo.dat");
}

module_check(write_file_should_fail_if_file_can_not_be_opened) {
  char* data = "0123456789";
  tarsio_mock.fopen.func = NULL;
  tarsio_mock.fopen.retval = NULL;
  assert_eq(WRITE_FILE_FOPEN_FAILED, write_file("/tmp/foo.dat", data, 10));
}

module_check(write_file_should_fail_if_file_can_not_be_written) {
  char* data = "0123456789";
  tarsio_mock.fwrite.func = NULL;
  tarsio_mock.fwrite.retval = 0;
  assert_eq(WRITE_FILE_FWRITE_FAILED, write_file("/tmp/foo.dat", data, 10));
  unlink("/tmp/foo.dat");
}

module_check(write_file_should_fail_if_file_can_not_be_closed) {
  char* data = "0123456789";
  tarsio_mock.fclose.func = NULL;
  tarsio_mock.fclose.retval = EOF;
  assert_eq(WRITE_FILE_FCLOSE_FAILED, write_file("/tmp/foo.dat", data, 10));
  unlink("/tmp/foo.dat");
}
