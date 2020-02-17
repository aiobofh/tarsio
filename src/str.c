/*
 * String helpers
 *
 *              _______          _____ ___        ______
 *                 |      ||    |         |    | |      |
 *                 |      ||    |         |    | |      |
 *                 |   ___||___ |         |___ | |______|
 *
 *              Copyright (C) 2020 AiO Secure Teletronics
 *
 * These string helpers are mainly written to make Tarsio a bit more portable,
 * since some string operations of the Posix standard (however convenient) are
 * not available on other systems.
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

#include <stdlib.h>
#include <string.h>

#include "error.h"

#include "str.h"

char* strclone(const char* src) {
  char *dst;

  if (NULL == (dst = malloc(strlen(src) + 1))) {
    error1("Out of memory while allocating string clone '%s'", src);
    goto malloc_failed;
  }

  if (NULL == strcpy(dst, src)) {
    error1("Could not copy string clone '%s'", src);
    goto strcpy_failed;
  }

  goto normal_exit;

 strcpy_failed:
  free(dst);
  dst = NULL;
 malloc_failed:
 normal_exit:
  return dst;
}
