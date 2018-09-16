#ifndef _TARSIO_H_
#define _TARSIO_H_
#include <assert.h>
#include <stdio.h>
#define tarsio_mock (*((__tarsio_data_t*)__tarsio_mock_data))
#define test(NAME) void __tarsio_test_##NAME(void* __tarsio_mock_data)
#define assert_eq(EXP, ACT) if ((EXP) != (ACT)) { fprintf(stderr, "FAILED! " #EXP " != " #ACT "\n"); }
struct __tarsio_data_s;

typedef struct __tarsio_data_s __tarsio_data_t;

void __tarsio_handle_arguments(int argc, char* argv[]);
void __tarsio_unit_test_execute(__tarsio_data_t* __tarsio_mock_data, int (*func)(void* __tarsio_mock_data), const char* name, size_t mock_data_size);
#endif
