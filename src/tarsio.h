#ifndef _TARSIO_H_
#define _TARSIO_H_
#include <assert.h>
#include <stdio.h>
#define tarsio_mock (*((__tarsio_data_t*)__tarsio_mock_data))
#define test(NAME) void __tarsio_test_##NAME(void* __tarsio_mock_data, const char* __tarsio_test_name)
#define assert_eq(EXP, ACT) __tarsio_assert_eq(((EXP) != (ACT)), __tarsio_test_name)
#define skip(REASON) __tarsio_skip(REASON, __tarsio_test_name); return;
struct __tarsio_data_s;

typedef struct __tarsio_data_s __tarsio_data_t;
void __tarsio_assert_eq(int res, const char* testcase_name);
void __tarsio_init(void);
void __tarsio_handle_arguments(int argc, char* argv[]);
void __tarsio_skip(const char* reason, const char* test_name);
void __tarsio_unit_test_execute(__tarsio_data_t* __tarsio_mock_data, int (*func)(void* __tarsio_mock_data, const char* __tarsio_test_name), const char* name, size_t mock_data_size);
void __tarsio_summary(void);
#endif
