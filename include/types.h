#ifndef TYPES_H
#define TYPES_H

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

typedef signed char int8_t;
typedef signed short int16_t;
typedef signed int int32_t;
typedef signed long long int64_t;

typedef uint32_t size_t;

#ifndef __cplusplus
#if !defined(__bool_true_false_are_defined) && !defined(__STDC_VERSION__) || (__STDC_VERSION__ < 202311L)
#ifndef bool
#define bool _Bool
#define true 1
#define false 0
#define __bool_true_false_are_defined 1
#endif
#endif
#endif

#ifndef NULL
#define NULL ((void*)0)
#endif

#endif

