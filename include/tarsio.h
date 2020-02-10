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
 */

#ifndef _TARSIO_H_
#define _TARSIO_H_
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define tarsio_mock (*((__tarsio_data_t*)__tarsio_mock_data))
#define test(NAME) void __##NAME(void* __tarsio_mock_data, const char* __tarsio_test_name)
#define assert_eq(EXP, ACT) __tarsio_assert_eq(((EXP) != (ACT)), __tarsio_test_name, #EXP " != " #ACT, __FILE__, __LINE__)
#define skip(REASON) __tarsio_skip(REASON, __tarsio_test_name); return;
struct __tarsio_data_s;

typedef struct __tarsio_data_s __tarsio_data_t;
void __tarsio_assert_eq(int res, const char* testcase_name, const char* help, const char* file, size_t line);
void __tarsio_init(void);
void __tarsio_handle_arguments(int argc, char* argv[]);
void __tarsio_skip(const char* reason, const char* test_name);
void __tarsio_unit_test_execute(__tarsio_data_t* __tarsio_mock_data, int (*func)(void* __tarsio_mock_data, const char* __tarsio_test_name), const char* name, size_t mock_data_size);
int __tarsio_summary(void);
void __tarsio_cleanup(void);
#endif
