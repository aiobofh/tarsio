/*
 * Check-suite for ../src/str.c
 *
 *              _______          _____ ___        ______
 *                 |      ||    |         |    | |      |
 *                 |      ||    |         |    | |      |
 *                 |   ___||___ |         |___ | |______|
 *
 *                   Copyleft AiO Secure Teletronics
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

#include "str.h"
#include "str_data.h"

#include "tarsio.h"

#include "helpers.h"

#define m tarsio_mock

/***************************************************************************
 * strclone()
 */
test(strclone_shall_malloc_enough_memory_for_string_copy) {
  m.STRLEN.func = strlen;
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
  assert_eq(1, m.STRCPY.call_count);
  assert_eq((char*)0x1234, m.STRCPY.args.arg0);
  assert_eq((char*)0x5678, m.STRCPY.args.arg1);
}

test(strclone_shall_free_memory_if_strcpy_failed) {
  m.malloc.retval = (void*)0x1234;
  strclone((const char*)0x5678);
  assert_eq(1, m.free.call_count);
  assert_eq((void*)0x1234, m.free.args.arg0);
}

test(strclone_shall_not_free_memory_on_success) {
  m.malloc.retval = (void*)0x1234;
  m.STRCPY.retval = m.malloc.retval;
  strclone((const char*)0x5678);
  assert_eq(0, m.free.call_count);
}

test(strclone_shall_return_the_new_string_pointer_on_success) {
  m.malloc.retval = (void*)0x1234;
  m.STRCPY.retval = m.malloc.retval;
  assert_eq((char*)0x1234, strclone((const char*)0x5678));
}
