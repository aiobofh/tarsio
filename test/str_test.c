#include "str.h"
#include "str_data.h"

#include "tarsio.h"

#define m tarsio_mock

/***************************************************************************
 * strclone()
 */
test(strclone_shall_malloc_enough_memory_for_string_copy) {
#ifdef SASC
  m.__builtin_strlen.func = strlen;
#else
  m.strlen.func = strlen;
#endif
  strclone("012345");
  assert_eq(1, m.malloc.call_count);
  assert_eq(7, m.malloc.args.arg0);
}

test(srtclone_shall_return_NULL_if_out_of_memory) {
  assert_eq(NULL, strclone("012345"));
}

test(strclone_shall_call_strcpy_correctly) {
  m.malloc.retval = (void*)0x1234;
  strclone((const char*)0x5678);
#ifdef SASC
  assert_eq(1, m.__builtin_strcpy.call_count);
  assert_eq((char*)0x1234, m.__builtin_strcpy.args.arg0);
  assert_eq((char*)0x5678, m.__builtin_strcpy.args.arg1);
#else
  assert_eq(1, m.strcpy.call_count);
  assert_eq((char*)0x1234, m.strcpy.args.arg0);
  assert_eq((char*)0x5678, m.strcpy.args.arg1);
#endif
}

test(strclone_shall_free_memory_if_strcpy_failed) {
  m.malloc.retval = (void*)0x1234;
  strclone((const char*)0x5678);
  assert_eq(1, m.free.call_count);
  assert_eq((void*)0x1234, m.free.args.arg0);
}

test(strclone_shall_not_free_memory_on_success) {
  m.malloc.retval = (void*)0x1234;
#ifdef SASC
  m.__builtin_strcpy.retval = m.malloc.retval;
#else
  m.strcpy.retval = m.malloc.retval;
#endif
  strclone((const char*)0x5678);
  assert_eq(0, m.free.call_count);
}

test(strclone_shall_return_the_new_string_pointer_on_success) {
  m.malloc.retval = (void*)0x1234;
#ifdef SASC
  m.__builtin_strcpy.retval = m.malloc.retval;
#else
  m.strcpy.retval = m.malloc.retval;
#endif
  assert_eq((char*)0x1234, strclone((const char*)0x5678));
}
