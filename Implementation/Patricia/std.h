#ifndef BITC_STD_H
#define BITC_STD_H

#include <stdint.h>
#include <stdio.h>

typedef uint8_t UInt8;
typedef int8_t SInt8;
typedef uint16_t UInt16;
typedef int16_t SInt16;
typedef uint32_t UInt32;
typedef int32_t SInt32;
typedef uint64_t UInt64;
typedef int64_t SInt64;

/**
 * Very simple logging macros.
 */

#ifndef NDEBUG
#define DEBUG(...) fprintf(stderr, __VA_ARGS__)
#else
#define DEBUG(...)
#endif

#define INFO(...)  fprintf(stderr, __VA_ARGS__)
#define WARN(...)  fprintf(stderr, __VA_ARGS__)
#define FATAL(...) fprintf(stderr, __VA_ARGS__)

#endif // BITC_STD_H
