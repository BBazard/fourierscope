/* Main Doxygen page */

/**
 * @mainpage FourierScope Project Reference
 *
 * @section intro Introduction
 *
 * This is the complete API reference for FourierScope
 *
 * @subsection require Requirements
 *
 * You need to have the followings installed :
 *
 * To build the fourierscope binary, you will need:
 * * a C compiler
 * * libfftw
 * * libtiff
 *
 * To build the tests binary, you will need:
 * * a C++ compiler
 * * libfftw
 * * libtiff
 * * libgtest
 *
 * @subsection build Building the binaries for fourierscope and tests
 *
 * * To build the binary issue the command "make" in the root directory
 * * To build the tests issue the command "make tests" in the root directory
 *
 * @subsection run To Run the binaries
 *
 * * Execute bin/fourierscope
 * * Execute bin/runtests for the tests (bin/runtests --help for help)
 *
 * @section testing How to use fourierscope and the tests
 *
 * @subsection subset Get some results
 *
 * * Build the tests (see previous section) and run bin/runtests --gtest_filter='-full/*'
 * * The previous commands will generate some basic output in the build directory
 * * The final images obtained are all prefixed with swarm_with_ followed by the parameters
 *   used for this image (j03 means jorga = 03, d50 means delta = 50, r40 means radius = 40)
 *
 * @subsection fullset Get extended results
 *
 * * Build the tests and run bin/runtests --gtest_filter='full/*'
 * * The previous command will generate a lot of outputs /!\ This may take a long time so be prepared or buy a powerfull proc /!\
 * * Same as before the results are in build directory
 *
 *
 * @section benchmarks Benchmarking
 *
 * * To benchmark, use the macros defined in release/include/benchmark.h
 * * Build the release version ("make") and execute it.
 * * Benchmark output will be displayed on stdout, one may redirect it to a file using
 *   shell redirections or the tee command.
 *
 */
