
# Microtest - C++ Single Header Unit Testing Library

This library has its focus on providing unit testing capabilities for
libraries and algorithms with minimal external dependencies, and straight
forward usage. The file `microtest.hh` MIT licensed and can therefore
be stored along with the code. Feature selection of the library (such
ANSI coloring, sequence generation, or temporary file creation) are
opt-in using compile-time switches.

### Implementing and Executing Tests

To execute a test, you create a test c++ implementation file, configure
and include the `microtest.hh`, implement the test, compile and run.
A log with `pass`, `fail`, `warn`, (also `info`/`note`) lines, as well
as a summary verdict will be printed to `stdout`. The exit code is `0`
on `PASS`, nonzero on `FAIL` (or if the test crashes).

A basic test file may look like the following listing:

  ```c++
  // @file my_test.cpp

  // (1) Include your code to test
  #include "..../my-code-to-test.hpp"
  #include <my-other-includes>

  // (2) Configure and include the testing library.
  //     Here we let microtest generate int main(){} with
  //     argument transfer, OS/compiler info, summary
  //     verdict, and exit code using `WITH_MICROTEST_MAIN`.
  #define WITH_MICROTEST_MAIN
  #define WITH_MICROTEST_ANSI_COLORS
  //     Presuming project root is in the include path.
  #include <test/microtest/include/microtest.hh>

  // (3) Write a test main function, and/or additional sub-tests.
  void test(const std::vector<std::string>& args)
  {
    // Comments / informational test log output.
    const char a = 'a';
    test_info("This is ", a, " variadic argument based logging.");
    test_info("Number of cli arguments: ", args.size());
    test_note("This is " << a << " ostream based information.");

    // Basic check examples
    test_expect(1 == 0);  // Will fail
    test_expect(1 == 1);  // Will pass
    test_expect_eq(0, 0); // Check equal, will pass
    test_expect_eq(0, 1); // Check equal, will fail
    test_expect_ne(1, 0); // Check not equal, will pass
    test_expect_ne(1, 1); // Check not equal, will fail
  }
  ```

Let's compile and run it, here with g++:

  ```sh
  # Linux/unix
  $ g++ -o my_test my_test.cpp -I. -W -Wall -Wextra -Werror -pedantic
  $ ./my_test

  # Windows
  $ g++ -o my_test my_test.cpp -I. -W -Wall -Wextra -Werror -pedantic -static
  $ my_test
  ```
  The output on a TTY will look similar to:

  <pre>
    <span style="color:blue;">[info]</span> <span style="color:teal;">[@test/microtest/include/microtest.hh:1395]</span> compiler: gcc (13.2.0), std=c++17, platform: linux
    <span style="color:gray;">[note]</span> <span style="color:teal;">[@my_test.cpp:21]</span> This is a variadic argument based logging.
    <span style="color:gray;">[note]</span> <span style="color:teal;">[@my_test.cpp:22]</span> Number of cli arguments: 0
    <span style="color:gray;">[note]</span> <span style="color:teal;">[@my_test.cpp:23]</span> This is a ostream based information.
    <span style="color:red;">[fail]</span> <span style="color:teal;">[@my_test.cpp:27]</span> i == 0
    <span style="color:green;">[pass]</span> <span style="color:teal;">[@my_test.cpp:28]</span> i == 1
    <span style="color:green;">[pass]</span> <span style="color:teal;">[@my_test.cpp:29]</span> 0 == 0   (=0)
    <span style="color:red;">[fail]</span> <span style="color:teal;">[@my_test.cpp:30]</span> 0 == i   (0 != 1)
    <span style="color:green;">[pass]</span> <span style="color:teal;">[@my_test.cpp:31]</span> i != 0   (1 != 0)
    <span style="color:red;">[fail]</span> <span style="color:teal;">[@my_test.cpp:32]</span> i != 1   (both =1)
    <span style="color:red;">[FAIL]</span> 3 of 6 checks failed, 0 warnings.
  </pre>

### Functionality and Compile Time Switches

The main c++ template based testing features are wrapped with a set of macros
(similar to `assert()`, to show file and line information). These are documented
in the main `microtest.hh` file.

During a test run, global statistics about how many checks have been done, how
many have succeeded or failed, and how many warnings were registered. Checks
implicitly alter/increment the `pass` and `fail` statistics. Warnings and notes
can be added programmatically.

#### Configuration Switches

Either via command line definition (e.g. with `-D`), or file-inline before
including the `microtest.hh` header, a set of configuration switches allow
you to specify the harness behavior and compile performance:

  - `WITH_MICROTEST_ANSI_COLORS` (or legacy `WITH_ANSI_COLORS`) enables coloring
    the output if the output channel is a TTY/console. File logging or piping
    will be without ANSI escape sequences.

  - `WITH_MICROTEST_MAIN` saves the repetition of creating a `main()` function
    and initializing, summary evaluation, and framing exception handling. The
    test entry point is then `void test(const vector<string>& args)`.

  - `WITH_MICROTEST_GENERATORS` enables data generation, like iterable
    sequence containers.

  - `MICROTEST_WITHOUT_PASS_LOGS` instructs the test logger to omit logging
    of `[pass]` lines to keep the log files smaller. The `[PASS]` verdict at
    the end will still be logged.

  - `WITHOUT_MICROTEST_RANDOM` omits the definition of random generators,
    which may have an effect on compile time performance.

  - (`WITH_MICROTEST_TMPFILE` ***experimental***, not generic) enables the
    additional features `const auto file = test_make_tmpfile();` and
    `const auto dir = test_make_tmpdir();`, which are cleaned up when the
    variable scope is left.

#### Verification Checking

Verification wrappers all contain the word `test_expect`, which marks that
a `pass`/`fail` decision is made and registered:

```c++

  /**
   * Checks (register pass/fail) and print the file+line, as well as the
   * expression formulating the check. Fails on exception.
   * @param BoolExpr...
   * @return bool
   */
  #define test_expect(...)

  /**
   * Registers a passed check if `A==B`, a failed check on `!(A==B)` (operator==()
   * match), and prints the file+line, the expression, and the value information
   * accordingly.
   * @tparam T1
   * @tparam T2
   * @param T1&& A
   * @param T1&& B
   * @return bool
   */
  #define test_expect_eq(A, B)

  /**
   * Registers a passed check if `A!=B`, a failed check on `!(A!=B)` (operator!=()
   * match), and prints the file+line, the expression, and the value information
   * accordingly.
   * @tparam T1
   * @tparam T2
   * @param T1&& A
   * @param T1&& B
   * @return bool
   */
  #define test_expect_ne(A, B)

  /**
   * Registers a passed check if `A>B`, a failed check on `!(A>B)` (operator>()
   * match), and prints the file+line, the expression, and the value information
   * accordingly.
   * @tparam T1
   * @tparam T2
   * @param T1&& A
   * @param T1&& B
   * @return bool
   */
  #define test_expect_gt(A, B)

  /**
   * Registers a passed check if `A<B`, a failed check on `!(A<B)` (operator<()
   * match), and prints the file+line, the expression, and the value information
   * accordingly.
   * @tparam T1
   * @tparam T2
   * @param T1&& A
   * @param T1&& B
   * @return bool
   */
  #define test_expect_lt(A, B)

  /**
   * Registers a passed check if `A>=B`, a failed check on `!(A>=B)` (operator>=()
   * match), and prints the file+line, the expression, and the value information
   * accordingly.
   * @tparam T1
   * @tparam T2
   * @param T1&& A
   * @param T1&& B
   * @return bool
   */
  #define test_expect_ge(A, B)

  /**
   * Registers a passed check if `A<=B`, a failed check on `!(A<=B)` (operator<=()
   * match), and prints the file+line, the expression, and the value information
   * accordingly.
   * @tparam T1
   * @tparam T2
   * @param T1&& A
   * @param T1&& B
   * @return bool
   */
  #define test_expect_le(A, B)

  /**
   * Registers a passed check if the given expression throws,
   * otherwise a failed check is registered. The result value
   * of the expression itself is ignored.
   * @param BoolExpr...
   * @return void
   */
  #define test_expect_except(...)

  /**
   * Registers a failed check if the given expression throws,
   * otherwise a passed check is registered. The result value
   * of the expression itself is ignored.
   * @param BoolExpr...
   * @return void
   */
  #define test_expect_noexcept(...)

  /**
   * Checks (register pass/fail) and print the file+line, as well as the
   * expression formulating the check. Does not catch exceptions (abort).
   * @param BoolExpr...
   * @return bool
   */
  #define test_expect_nocatch(...)

  /**
   * Checks (register pass/fail) and print the file+line, as well as the
   * expression formulating the check. Does not catch exceptions.
   * Legacy alias of `test_expect_nocatch()`.
   * @param BoolExpr...
   * @return bool
   */
  #define test_expect_cond(...)

  /**
   * Checks (register pass/fail) without printing the line
   * (use for highly iterated/nested check sequences).
   * Does not catch exceptions (test run aborted).
   * @see ::sw::utest::test::omit_pass_log(bool enable)
   * @param BoolExpr...
   * @return bool
   */
  #define test_expect_cond_silent(...)

  /**
   * Checks (register pass/fail) without printing the line
   * (use for highly iterated/nested check sequences).
   * @param BoolExpr...
   * @return bool
   */
  #define test_expect_silent(...)

```

#### Explicit Registering and Logging

For registering success, fails, warnings, or logs in the test code, the
following wrapper macros (of variadic template functions) can be used:

```c++

  /**
   * Register a failed check with message (arguments implicitly printed space separated).
   * @tparam typename ...Args
   * @return void
   */
  #define test_fail(...)

  /**
   * Register a passed check with message (arguments implicitly printed space separated).
   * @tparam typename ...Args
   * @return void
   */
  #define test_pass(...)

  /**
   * Register a check warning (arguments implicitly printed space separated).
   * @tparam typename ...Args
   * @return void
   */
  #define test_warn(...)

  /**
   * Print information without registering a check (arguments implicitly printed space separated).
   * @tparam typename ...Args
   * @return void
   */
  #define test_info(...)

  /**
   * Print information without registering a check (arguments implicitly printed space separated).
   * Alias of `test_info(...)`.
   * @tparam typename ...Args
   * @return void
   */
  #define test_comment(...)

  /**
   * Print information without registering a check (argument stream like).
   * e.g.: test_note("Check loop " << long(current_iteration) << " ...");
   * @tparam typename ...Args
   * @return void
   */
  #define test_note(...)

```

#### Test Harness Control

For self-written `int main(){}` functions, the test harness has to be initialized,
and at the end, `return test_summary();` can be used to print the verdict and terminate
with the corresponding exit code. Additionally, build information can be logged,
and the test statistics can be reset:

```c++

  /**
   * Initialize the test run, print build context information, and
   * conditionally enable ANSI coloring if allowed (WITH_MICROTEST_ANSI_COLORS
   * defined and the output stream STDOUT is a TTY/console).
   * @see `WITH_MICROTEST_MAIN`
   */
  #define test_initialize()

  /**
   * Print build context information.
   * @see `WITH_MICROTEST_MAIN`
   * @return void
   */
  #define test_buildinfo()

  /**
   * Prints the summary of all test (statistics since the last `test_reset()`
   * or the beginning). Returns a suitable exit code for the `int main(){}`,
   * indicating test success (exit code 0) or fail (exit code >0).
   * @see `WITH_MICROTEST_MAIN`
   * @return int
   */
  #define test_summary()

  /**
   * Reset the test statistics (number of fails, passes, and warnings).
   * @return void
   */
  #define test_reset()

```

#### Test Data Generators

Especially for fuzzing, random value generation is provided. Optionally
sequence generators can be added via compile time configuration:

```c++

  /**
   * Random value or container generation (uniform distribution).
   * - test_random<double>()         : double between 0..1
   * - test_random<long double>(10)  : long double between 0..10
   * - test_random<float>(-1, 1)     : float between -1 and 1
   * - test_random<int>(100)         : integer between 0 and 100
   * - test_random<int>()            : integer between INT_MIN and INT_MAX
   * - test_random<string>(10)       : std::string with 10 characters, values ASCII, between space (32) and '~' (126).
   * - test_random<vector<double>>(5,0,1): double vector with 5 entries and a value range between 0 and 1.
   * - test_random<deque<int>>(5,0,1):  deque with 5 entries, int type, and a value range between 0 and 1.
   */
  #define test_random ::sw::utest::random

  /**
   * Generates a sequential value test vector, with
   * a given element type T, a size N, and with the
   * first value being `start_value`.
   * @tparam typename T
   * @tparam size_t N
   * @return std::vector<T>
   */
  #define test_sequence_vector ::sw::utest::sequence_vector

  /**
   * Generates a sequential value test array, with
   * a given element type T, a size N, and with the
   * first value being `start_value`.
   * @tparam typename T
   * @tparam size_t N
   * @return std::array<T,N>
   */
  #define test_sequence_array ::sw::utest::sequence_array

```

### Standards and Compilers

The harness was started with `c++11`, and ported to `c++14`, `c++17`,
and `c++20` with backward-compatibility. Compilers tested with are
mainly g++ and clang++.

### Example Repository Structure

Although independent of any structure and build system, an example
using GNU `Makefile`s is depicted in this project repository.

  ```
    ./
     |- Makefile
     \- test/
        |- testenv.mk
        |- testenv.hh
        |- microtest/
        |  |- include/microtest.hh
        |  \- test.cc
        |
        |- test_one/test.cc
        |- test_two/test.cc
        |- ...
  ```

The `microtest.hh` file is located in `./test/microtest/include`. Other
directories in the `./test/` are test collections and shall contain
`test.cc` files.

The root `Makefile` includes `test/testenv.mk`, providing the targets:

  - `make test`: Compile and run all tests (files `./test/*/test.cc`).

  - `make test-clean`: Cleanup tests in `./build`.

  - `make coverage`: ***Linux/unix only***, requires `gcov` and `lcov`
    installed.

A common test environment header `testenv.hh` contains the common harness
config and includes `microtest.hh`, and is included in the `test.cc` files.

The executables, results and coverage information are located in the
`./build` directory, and the summary verdicts printed to stdout.

The compiler settings are defined for the root make file, as often
done for projects (`CXX`, `LD`, `CXXFLAGS`, `CXX_STD` etc).

Example usage of the root makefile is:

```sh
  # Compile && run tests, parallel build (default g++ used)
  $ make

  # Compile && run tests with different c++ standards, parallel build
  $ make all

  # Compile && run a single test `t0001`
  $ make test TEST=0001

  # Compile && run with clang
  $ make test CXX=clang++

  # Clean, rebuild with coverage using gcov/lcov
  $ make coverage
```


---
