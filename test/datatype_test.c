/*
 * Check-suite for ../src/datatype.c
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
#include <stdlib.h>

#include "datatype.h"
#include "datatype_data.h"
#include "tarsio.h"

#define m tarsio_mock

/***************************************************************************
 * datatype_init()
 */
test(datatype_init_shall_set_all_relevant_data) {
  datatype_t dt;
  unsigned char* ref = (unsigned char*)&dt;
  const unsigned char* endptr = ref + sizeof(dt) - 4;
  memset(&dt, 0, sizeof(dt));
  datatype_init(&dt, (char*)-1, -1, -1, -1, -1, -1);
  /* Check only the datatype definitions */
  ref += (sizeof(datatype_t) - sizeof(datatype_definition_t));
  while (endptr > ref) {
    assert_eq(0, (*ref == 0));
    ref++;
  }
}

/***************************************************************************
 * datatype_cleanup()
 */
test(datatype_cleanup_shall_free_datatype_name) {
  datatype_t dt;
  dt.name = (char*)1234;

  datatype_cleanup(&dt);

  assert_eq(1, m.free.call_count);
  assert_eq((char*)1234, m.free.args.arg0);
}

test(datatype_cleanup_shall_not_free_datatype_name_if_null) {
  datatype_t dt;
  dt.name = NULL;

  datatype_cleanup(&dt);

  assert_eq(0, m.free.call_count);
}
