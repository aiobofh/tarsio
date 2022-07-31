/*
 * Error-output macros
 *
 *              _______          _____ ___        ______
 *                 |      ||    |         |    | |      |
 *                 |      ||    |         |    | |      |
 *                 |   ___||___ |         |___ | |______|
 *
 *            Copyright (C) 2020-2022 AiO Secure Teletronics
 *
 * This file contains macros for outputing (and documenting) error messages.
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
#ifndef _ERROR_H_
#define _ERROR_H_

#include <stdio.h>

/*
 * Since Tarsio is meant to be highly portable, variadic macros are not
 * used here. It would be more convenient, but it is what it is.
 */

#define error0(STR) fprintf(stderr, "%s:%d ERROR: " STR "\n", __FILE__, __LINE__)
#define error1(FMT, ARG1) fprintf(stderr, "%s:%d ERROR: " FMT "\n", __FILE__, __LINE__, ARG1)
#define error2(FMT, ARG1, ARG2) fprintf(stderr, "%s:%d ERROR: " FMT "\n", __FILE__, __LINE__, ARG1, ARG2)
#define error3(FMT, ARG1, ARG2, ARG3) fprintf(stderr, "%s:%d ERROR: " FMT "\n", __FILE__, __LINE__, ARG1, ARG2, ARG3)
#define error4(FMT, ARG1, ARG2, ARG3, ARG4) fprintf(stderr, "%s:%d ERROR: " FMT "\n", __FILE__, __LINE__, ARG1, ARG2, ARG3, ARG4)

#endif
