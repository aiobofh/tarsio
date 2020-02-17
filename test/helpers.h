/*
 * Helper macros for the check-suites
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

#ifndef _HELPERS_H_
#define _HELPERS_H_

#ifdef SASC
# define MEMSET __builtin_memset
# define STRLEN __builtin_strlen
# define STRCPY __builtin_strcpy
# define ASSERT __assert
#endif
#ifdef VBCC
# define MEMSET memset
# define STRLEN strlen
# define STRCPY strcpy
# define ASSERT abort
#endif
#ifdef VC
# define MEMSET memset
# define STRLEN strlen
# define STRCPY strcpy
# define ASSERT _wassert
#endif
#ifndef MEMSET
# define MEMSET memset
# define STRLEN strlen
# define STRCPY strcpy
# define ASSERT __assert_fail
#endif

#endif
