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
