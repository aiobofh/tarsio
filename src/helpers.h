#ifndef _HELPERS_H_
#define _HELPERS_H_

#include "debug.h"
#include "error.h"

#include <assert.h>

/*****************************************************************************
 * Some fast string compare macros
 */
#if UINTPTR_MAX == 0xffffffff
#else
#define Q64BIT
#endif

#define U08 uint8_t
#define U16 uint16_t
#define U32 uint32_t
#ifdef Q64BIT
#define U64 uint64_t
#endif

#define _q1(H,N) ((*(U08*)&(H)[0] ^ *(U08*)&(N)[0]))
#define _q2(H,N) ((*(U16*)&(H)[0] ^ *(U16*)&(N)[0]))
#define _q3(H,N) (_q2(&(H)[0],&(N)[0]) | _q1(&(H)[2],&(N)[2]))
#define _q4(H,N) ((*(U32*)&(H)[0] ^ *(U32*)&(N)[0]))
#define _q5(H,N) (_q4(&(H)[0],&(N)[0]) | _q1(&(H)[4],&(N)[4]))
#define _q6(H,N) (_q4(&(H)[0],&(N)[0]) | _q2(&(H)[4],&(N)[4]))
#define _q7(H,N) (_q4(&(H)[0],&(N)[0]) | _q3(&(H)[4],&(N)[4]))
#ifdef Q64BIT
#define _q8(H,N) ((*(U64*)&(H)[0] ^ *(U64*)&(N)[0]))
#else
#define _q8(H,N) (_q4(&(H)[0],&(N)[0]) | _q4(&(H)[4],&(N)[4]))
#endif
#define _q9(H,N) (_q8(&(H)[0],&(N)[0]) | _q1(&(H)[8],&(N)[8]))
#define _q10(H,N) (_q8(&(H)[0],&(N)[0]) | _q2(&(H)[8],&(N)[8]))
#define _q11(H,N) (_q8(&(H)[0],&(N)[0]) | _q3(&(H)[8],&(N)[8]))
#define _q12(H,N) (_q8(&(H)[0],&(N)[0]) | _q4(&(H)[8],&(N)[8]))
#define _q13(H,N) (_q8(&(H)[0],&(N)[0]) | _q5(&(H)[8],&(N)[8]))
#define _q14(H,N) (_q8(&(H)[0],&(N)[0]) | _q6(&(H)[8],&(N)[8]))
#define _q15(H,N) (_q8(&(H)[0],&(N)[0]) | _q7(&(H)[8],&(N)[8]))
#define _q16(H,N) (_q8(&(H)[0],&(N)[0]) | _q8(&(H)[8],&(N)[8]))
#define _q17(H,N) (_q16(&(H)[0],&(N)[0]) | _q1(&(H)[16],&(N)[16]))
#define _q18(H,N) (_q16(&(H)[0],&(N)[0]) | _q2(&(H)[16],&(N)[16]))
#define _q19(H,N) (_q16(&(H)[0],&(N)[0]) | _q3(&(H)[16],&(N)[16]))
#define _q20(H,N) (_q16(&(H)[0],&(N)[0]) | _q4(&(H)[16],&(N)[16]))
#define _q21(H,N) (_q16(&(H)[0],&(N)[0]) | _q5(&(H)[16],&(N)[16]))
#define _q22(H,N) (_q16(&(H)[0],&(N)[0]) | _q6(&(H)[16],&(N)[16]))
#define _q23(H,N) (_q16(&(H)[0],&(N)[0]) | _q7(&(H)[16],&(N)[16]))
#define _q24(H,N) (_q16(&(H)[0],&(N)[0]) | _q8(&(H)[16],&(N)[16]))
#define _q25(H,N) (_q16(&(H)[0],&(N)[0]) | _q9(&(H)[16],&(N)[16]))
#define _q26(H,N) (_q16(&(H)[0],&(N)[0]) | _q10(&(H)[16],&(N)[16]))
#define _q27(H,N) (_q16(&(H)[0],&(N)[0]) | _q11(&(H)[16],&(N)[16]))
#define _q28(H,N) (_q16(&(H)[0],&(N)[0]) | _q12(&(H)[16],&(N)[16]))
#define _q29(H,N) (_q16(&(H)[0],&(N)[0]) | _q13(&(H)[16],&(N)[16]))
#define _q30(H,N) (_q16(&(H)[0],&(N)[0]) | _q14(&(H)[16],&(N)[16]))
#define _q31(H,N) (_q16(&(H)[0],&(N)[0]) | _q15(&(H)[16],&(N)[16]))

#ifndef NDEBUG
#define _qstrncmp_assert(S,N) | qstrncmp_assert(S,N)
#else
#define _qstrncmp_assert(S,N)
#endif
#define qstrncmp(H,N,S) (_q##S(H,N) _qstrncmp_assert(sizeof(N) - 1 - S, N))
#ifndef NDEBUG
#ifndef STRNCMP_ASSERT
#define STRNCMP_ASSERT
static inline int qstrncmp_assert(int s, const char* needle) {
  if (s) {
    error1("Wrong size specified for '%s'", needle);
  }
  assert(!s && "Wrong constant size");
  return 0;
}
#else
extern static inline int qstrncmp_assert(int s);
#endif
#endif

/*****************************************************************************
 * AiO C-extension macros, the usual stuff ;)
 */
#define xstr(s) str(s)
#define str(s) #s
extern int __debug;
#define ret(LABEL) do {      \
    debug0(#LABEL);          \
    retval = LABEL;          \
    goto LABEL;              \
  } while (0)
#ifndef UNSAFE
#define __nullsafe if (NULL != __safep) __safep = NULL;
#define __intsafe if (0 == __safei) __safei = 0;
#define __negsafe if (0 < __safen) __safen = -1;
#define __safepcall(FUNCCALL) __safep = FUNCCALL; __nullsafe
#define __safeicall(FUNCCALL) __safei = FUNCCALL; __intsafe
#define __safencall(FUNCCALL) __safen = FUNCCALL; __negsafe
#else
#define __safepcall(FUNCCALL) FUNCCALL; if (1 == 1) __safei = 0;
#define __safeicall(FUNCCALL) FUNCCALL; if (1 == 1) __safei = 0;
#define __safencall(FUNCCALL) FUNCCALL; if (1 == 1) __safen = 0;
#endif
extern void* __safep;
extern int __safei;
extern int __safen;

/*****************************************************************************
 * Saft wrappers for standard C functions
 */
#define smalloc(size) __safepcall(malloc(size))

#endif
