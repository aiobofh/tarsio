#include "file.h"
#include "file_data.h"

#include "tarsio.h"

#define m tarsio_mock

/*****************************************************************************
 * fsize()
 */
test(fsize_shall_use_fseek_the_end_of_the_provided_file_as_base) {
  fsize((FILE*)0x1234);
  assert_eq(1, m.fseek.call_count);
  assert_eq((FILE*)0x1234, m.fseek.args.arg0);
  assert_eq(0L, m.fseek.args.arg1);
  assert_eq(SEEK_END, m.fseek.args.arg2);
}

test(fsize_shall_use_ftell_to_determine_the_number_of_bytes) {
  fsize((FILE*)0x1234);
  assert_eq(1, m.ftell.call_count);
  assert_eq((FILE*)0x1234, m.ftell.args.arg0);
}

test(fsize_shall_rewind_to_beginning_of_file_when_done) {
  fsize((FILE*)0x1234);
  assert_eq(1, m.rewind.call_count);
  assert_eq((FILE*)0x1234, m.rewind.args.arg0);
}

test(fsize_shall_forward_ftell_return_value) {
  m.ftell.retval = 5678;
  assert_eq(5678, fsize((FILE*)0x1234));
}

/*****************************************************************************
 * file_init()
 */
test(init_shall_open_the_correct_file_for_reading) {
  file_init((file_t*)0x1234, (char*)0x5678);
  assert_eq(1, m.fopen.call_count);
  assert_eq((char*)0x5678, m.fopen.args.arg0);
  assert_eq(0, strcmp("r", m.fopen.args.arg1));
}

test(init_shall_return_1_negative_if_file_can_not_be_opened) {
  assert_eq(-1, file_init((file_t*)0x1234, (char*)0x5678));
}

test(init_shall_get_the_file_size_of_the_correct_file) {
  m.fopen.retval = (FILE*)0x1234;
  file_init((file_t*)0x4321, (char*)0x5678);
  assert_eq(1, m.fsize.call_count);
  assert_eq((FILE*)0x1234, m.fsize.args.arg0);
}

test(init_shall_return_2_negative_if_filesize_is_zero) {
  m.fopen.retval = (FILE*)0x1234;
  assert_eq(-2, file_init((file_t*)0x1234, (char*)0x5678));
}

test(init_shall_close_file_if_size_is_zero) {
  m.fopen.retval = (FILE*)0x4321;
  file_init((file_t*)0x1234, (char*)0x5678);
  assert_eq(1, m.fclose.call_count);
  assert_eq((FILE*)0x4321, m.fclose.args.arg0);
}

test(init_shall_allocate_enough_memory_for_the_whole_file) {
  m.fopen.retval = (FILE*)0x1234;
  m.fsize.retval = 5678;
  file_init((file_t*)0x4321, (char*)0x5678);
  assert_eq(1, m.malloc.call_count);
  assert_eq(5678 + 1, m.malloc.args.arg0);
}

test(init_shall_return_3_negative_if_out_of_memory) {
  m.fopen.retval = (FILE*)0x1234;
  m.fsize.retval = 5678;
  assert_eq(-3, file_init((file_t*)0x4321, (char*)0x8765));
}

test(init_shall_read_the_correct_file_to_the_correct_memory) {
  m.fopen.retval = (FILE*)0x1234;
  m.fsize.retval = 5678;
  m.malloc.retval = (void*)0x4321;
  file_init((file_t*)0x1111, (char*)0x2222);
  assert_eq(1, m.fread.call_count);
  assert_eq((void*)0x4321, m.fread.args.arg0);
  assert_eq(5678, m.fread.args.arg2);
  assert_eq((FILE*)0x1234, m.fread.args.arg3);
}

test(init_shall_return_4_negative_if_file_is_unreadable) {
  m.fopen.retval = (FILE*)0x1234;
  m.fsize.retval = 5678;
  m.malloc.retval = (void*)0x4321;
  assert_eq(-4, file_init((file_t*)0x1111, (char*)0x2222));
}

test(init_shall_free_allocated_memory_on_read_failure) {
  m.fopen.retval = (FILE*)0x1234;
  m.fsize.retval = 5678;
  m.malloc.retval = (void*)0x4321;
  file_init((file_t*)0x1111, (char*)0x2222);
  assert_eq(1, m.free.call_count);
  assert_eq((void*)0x4321, m.free.args.arg0);
}

test(init_shall_close_file_read_failure) {
  m.fopen.retval = (FILE*)0x1234;
  m.fsize.retval = 5678;
  m.malloc.retval = (void*)0x4321;
  file_init((file_t*)0x1111, (char*)0x2222);
  assert_eq(1, m.fclose.call_count);
  assert_eq((FILE*)0x1234, m.fclose.args.arg0);
}

test(init_shall_return_0_on_success) {
  file_t file;
  m.fopen.retval = (FILE*)0x1234;
  m.fsize.retval = 5678;
  m.malloc.retval = (void*)0x4321;
  m.fread.retval = m.fsize.retval;
  assert_eq(0, file_init((file_t*)&file, (char*)0x2222));
}

test(init_shall_close_file_on_success) {
  file_t file;
  m.fopen.retval = (FILE*)0x1234;
  m.fsize.retval = 5678;
  m.malloc.retval = (void*)0x4321;
  m.fread.retval = 8765;
  file_init((file_t*)&file, (char*)0x2222);
  assert_eq(1, m.fclose.call_count);
  assert_eq((FILE*)0x1234, m.fclose.args.arg0);
}

test(init_shall_load_the_file_into_the_corrct_memory) {
  file_t file;
  m.fopen.retval = (FILE*)0x1234;
  m.fsize.retval = 5678;
  m.malloc.retval = (void*)0x4321;
  m.fread.retval = 8765;
  file_init((file_t*)&file, (char*)0x2222);
  assert_eq(file.buf, m.malloc.retval);
}

/*****************************************************************************
 * file_cleanup()
 */
