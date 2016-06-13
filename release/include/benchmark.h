/* Copyright [2016] <Alexis Lescouet, Benoît Bazard> */
/**
 *  @file
 *
 *  Benchmark functions header
 *
 */

#ifndef RELEASE_INCLUDE_BENCHMARK_H_
#define RELEASE_INCLUDE_BENCHMARK_H_

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <inttypes.h>
#include <time.h>

/**
 * For local usage.
 * To bench function test(int a, int b) do in place
 * of a test call:
 *
   \verbatim
   locb(test, 4, 5);
   \endverbatim
 *
 */
#define locb(fn, ...) { struct timespec start;                          \
    clock_gettime(CLOCK_MONOTONIC, &start);                             \
    fn(__VA_ARGS__);                                                    \
    struct timespec stop;                                               \
    clock_gettime(CLOCK_MONOTONIC, &stop);                              \
    uint64_t res = ((uint64_t)stop.tv_sec - (uint64_t)start.tv_sec)*((uint64_t)1000000000) + ((uint64_t)stop.tv_nsec - (uint64_t)start.tv_nsec); \
    printf("%s, line %d: %" PRIu64 "\n", #fn, __LINE__, (res/((uint64_t) 1000))); }

/**
 * @cond DEV
 * Used in combination with next function, allows two groups of
 * __VA_ARGS__ to be distinct when using glob macro
 *
 */
#define _(...) __VA_ARGS__
/** @endcond */

/**
 * For global usage.
 * To bench function test(int a, int b) do the following
 * outside any function:
 *
   \verbatim
   glob(test, _(int a, int b), a, b);
   \endverbatim
 *
 * This will create a function _test (or the name of the function).
 * 
 * Then by doing: 
 *
   \verbatim
   #undef test
   void test(int a, int b);
   #define test _test
   \endverbatim
 *
 * in the header file and:
 *
   \verbatim
   #undef test
   void test(int a, int b) {
   function body
   }
   #define test _test
   \endverbatim
 *
 * Each call to test would be replaced by the benched version
 *
 * Else, one may use the _test function.
 *
 */
#define glob(fn, args, ...) void _##fn(args) {                          \
    struct timespec start;                                              \
    clock_gettime(CLOCK_MONOTONIC, &start);                             \
    fn(__VA_ARGS__);                                                    \
    struct timespec stop;                                               \
    clock_gettime(CLOCK_MONOTONIC, &stop);                              \
    uint64_t res = ((uint64_t)stop.tv_sec - (uint64_t)start.tv_sec)*((uint64_t)1000000000) + ((uint64_t)stop.tv_nsec - (uint64_t)start.tv_nsec); \
    printf("%s, line %d: %" PRIu64 "\n", #fn, __LINE__, (res/((uint64_t) 1000))); \
  }

#endif /* RELEASE_INCLUDE_BENCHMARK_H_ */
