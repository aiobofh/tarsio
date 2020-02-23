/*
 * Example program that can be tested with Tarsio
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

#ifndef _WRITE_FILE_H_
#define _WRITE_FILE_H_

enum write_file_retval_e
  {
   WRITE_FILE_EVERYTHING_IS_OK = 0,
   WRITE_FILE_FOPEN_FAILED = -1,
   WRITE_FILE_FCLOSE_FAILED = -2,
   WRITE_FILE_FWRITE_FAILED = -3
  };
typedef enum write_file_retval_e write_file_retval_t;

#endif
