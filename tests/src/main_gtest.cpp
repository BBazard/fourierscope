/* Copyright [2016] <Alexis Lescouet, Benoît Bazard> */
/**
 *  @file
 *
 *  Main Test file
 *
 */

#include "gtest/gtest.h"

/**
 *  @brief main gtest function
 *
 *  This function calls all the tests suites, and can be used with
 *  differents options, see "binary name" --help to get help.
 *
 */
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
