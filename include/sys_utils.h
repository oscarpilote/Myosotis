#ifndef _SYS_UTILS_H
#define _SYS_UTILS_H

/* C++ does not allow implicitit cast of void*   */
/* This will work with gcc or clang              */
#if defined(__cplusplus)
	#define VOIDSTARCAST(x) (typeof (x))
#else
	#define VOIDSTARCAST(x) 
#endif

/* Branch prediction */
#if defined(__GNUC__) || defined(__clang__)
	#define LIKELY(x) (__builtin_expect((x), 1))
	#define UNLIKELY(x) (__builtin_expect((x), 1))
	#define ASSUME(cond) do { if (!(cond)) __builtin_unreachable(); } while (0)
	#define ASSUME_ALIGNED(var, size) do { var = VOIDSTARCAST(var) __builtin_assume_aligned(var, size); } while (0)
#else
	#define LIKELY(x) (x)
	#define UNLIKELY(x) (x)
	#define ASSUME(cond)
	#define ASSUME_ALIGNED(var, n)
#endif

// Remove old 16bit Windows.h macros near and far
#ifdef far
#undef far
#endif
#ifdef near
#undef near
#endif

#endif

