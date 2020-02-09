#ifndef _HELPERS_H_
#define _HELPERS_H_

#ifdef SASC
# define MEMSET __builtin_memset
# define STRLEN __builtin_strlen
# define STRCPY __builtin_strcpy
# define ASSERT __assert
#else
# ifdef VBCC
#  define MEMSET memset
#  define STRLEN strlen
#  define STRCPY strcpy
#  define ASSERT abort
# else
#  define MEMSET memset
#  define STRLEN strlen
#  define STRCPY strcpy
#  define ASSERT __assert_fail
# endif
#endif

#endif
