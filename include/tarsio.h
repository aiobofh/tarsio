/*
 * The Tarsio API
 *
 *              _______          _____ ___        ______
 *                 |      ||    |         |    | |      |
 *                 |      ||    |         |    | |      |
 *                 |   ___||___ |         |___ | |______|
 *
 *                   Copyleft AiO Secure Teletronics
 *
 * API declarations for check-suites. This file should be incldued in the top
 * of your check source file.
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

#ifndef _TARSIO_H_
#define _TARSIO_H_

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Easy access to mock-up data structs
 *
 * Example::
 *
 *   tarsio_mock.malloc.retval = (void*)NULL;
 *
 */
#define tarsio_mock (*((__tarsio_data_t*)__tarsio_mock_data))

/*
 * Efficient check-function header
 *
 * Example:
 *
 * test(this_is_my_test_name) {
 *   // ... My test-implementation
 * }
 *
 */
#define test(NAME) void __##NAME(void* __tarsio_mock_data, const char* __tarsio_test_name)

/*
 * Main check assertion macro to evaluate code
 *
 * Example:
 *
 * Let's assume my_function() somewhere will pass the value of the first
 * arugment directly to malloc. This can be checked like this:
 *
 *   my_function(10);
 *   assert_eq(tarsio_mock.malloc.args.arg0, 10);
 *
 */
#define assert_eq(EXP, ACT) __tarsio_assert_eq(((EXP) != (ACT)), __tarsio_test_name, #EXP " != " #ACT, __FILE__, __LINE__)

/*
 * Define a check as skipped using this macro
 *
 * Example::
 *
 *   skip("This is skipped because I'm lazy!");
 *
 */
#define skip(REASON) __tarsio_skip(REASON, __tarsio_test_name); return;

struct __tarsio_data_s;
typedef struct __tarsio_data_s __tarsio_data_t;

/*
 * The real function API called either by the macros declared above, or by
 * the check-runner. A "hidden" API that you as programmer will not have to
 * care too much about.
 */
void __tarsio_assert_eq(int res, const char* testcase_name, const char* help, const char* file, size_t line);
void __tarsio_init(void);
void __tarsio_handle_arguments(int argc, char* argv[]);
void __tarsio_skip(const char* reason, const char* test_name);
void __tarsio_unit_test_execute(__tarsio_data_t* __tarsio_mock_data, int (*func)(void* __tarsio_mock_data, const char* __tarsio_test_name), const char* name, size_t mock_data_size);
int __tarsio_summary(void);
void __tarsio_cleanup(void);

#endif
