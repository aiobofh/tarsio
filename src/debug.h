/*
 * Debug-output macros
 *
 *              _______          _____ ___        ______
 *                 |      ||    |         |    | |      |
 *                 |      ||    |         |    | |      |
 *                 |   ___||___ |         |___ | |______|
 *
 *            Copyright (C) 2020-2022 AiO Secure Teletronics
 *
 * This file contains macros for outputing (and documenting) debug
 * information.
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
#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <stdio.h>

/*
 * Since Tarsio is meant to be highly portable, variadic macros are not
 * used here. It would be more convenient, but it is what it is.
 */

#ifdef NODEBUG
#define debug0(STR)
#define debug1(FMT, ARG1)
#define debug2(FMT, ARG1, ARG2)
#define debug3(FMT, ARG1, ARG2, ARG3)
#define debug4(FMT, ARG1, ARG2, ARG3, ARG4)
#define debug5(FMT, ARG1, ARG2, ARG3, ARG4, ARG5)
#define debug6(FMT, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6)
#define debug7(FMT, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7)
#else
#define debug0(STR) if (__tarsio_debug_print) { fprintf(stderr, "DEBUG: " STR "\n"); }
#define debug1(FMT, ARG1) if (__tarsio_debug_print) { fprintf(stderr, "DEBUG: " FMT "\n", ARG1); }
#define debug2(FMT, ARG1, ARG2) if (__tarsio_debug_print) { fprintf(stderr, "DEBUG: " FMT "\n", ARG1, ARG2); }
#define debug3(FMT, ARG1, ARG2, ARG3) if (__tarsio_debug_print) { fprintf(stderr, "DEBUG: " FMT "\n", ARG1, ARG2, ARG3); }
#define debug4(FMT, ARG1, ARG2, ARG3, ARG4) if (__tarsio_debug_print) { fprintf(stderr, "DEBUG: " FMT "\n", ARG1, ARG2, ARG3, ARG4); }
#define debug5(FMT, ARG1, ARG2, ARG3, ARG4, ARG5) if (__tarsio_debug_print) { fprintf(stderr, "DEBUG: " FMT "\n", ARG1, ARG2, ARG3, ARG4, ARG5); }
#define debug6(FMT, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6) if (__tarsio_debug_print) { fprintf(stderr, "DEBUG: " FMT "\n", ARG1, ARG2, ARG3, ARG4, ARG5, ARG6); }
#define debug7(FMT, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7) if (__tarsio_debug_print) { fprintf(stderr, "DEBUG: " FMT "\n", ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7); }
#endif

extern int __tarsio_debug_print;

#endif
