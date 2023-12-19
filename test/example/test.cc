/**
 * @test example
 *
 * Example usages of the functionality provided in `microtest.hh`.
 */
#define WITH_ANSI_COLORS
#define WITH_MICROTEST_GENERATORS
#include <test/microtest/include/microtest.hh>

#include <iostream>
#include <sstream>
#include <vector>
#include <deque>
#include <list>

using namespace std;

// Auxiliary function that throws a runtime error
bool throws_exception(bool e)
{
  if(e) throw std::runtime_error("Bad something");
  return false;
}

// Auxiliary forward iterable container --> string
// Not in the microtest harness, as overrides may
// collide with existing to_string() implementations.
template<typename ContainerType>
string to_string(ContainerType c)
{
  if(c.empty()) return "[]";
  stringstream s;
  auto it = c.begin();
  s << "[ " << *it;
  for(++it; it != c.end(); ++it) { s << ", " << *it; }
  s << " ]";
  return s.str();
}

//
// Example test main function (for `int main()` see below).
//
void test(const vector<string>& args)
{
  // Optional, only here for convenience.
  using namespace std;
  using namespace sw::utest;

  // Comments / informational test log output.
  char a = 'a';
  string s = "test";
  test_info("This is ", a, " ", s, ".");          // variadic template based information
  test_note("This is " << a << " " << s << ".");  // ostream based information
  test_info("Test args are:", to_string(args));

  // Basic checks
  test_expect(1 == 0);   // Will fail
  test_expect(1 == 1);   // Will pass
  test_expect_eq(0, 0);  // Check equal, will pass
  test_expect_eq(0, 1);  // Check equal, will fail
  test_expect_ne(1, 0);  // Check not equal, will pass
  test_expect_ne(1, 1);  // Check not equal, will fail

  // Exception
  test_expect(throws_exception(true));            // Will fail due to exception
  test_expect_except(throws_exception(true));     // Will pass, expected exception
  test_expect_noexcept(throws_exception(false));  // Will pass, no exception expected

  // Random string
  test_info("Random string: ", test_random<string>(10));

  // Random arithmetic
  test_info("Random char: ", int(test_random<char>()));
  test_info("Random uint8: ", int(test_random<std::uint8_t>(100)));
  test_info("Random double: ", test_random<double>());       // 0 to 1
  test_info("Random double: ", test_random<double>(10));     // 0 to 10
  test_info("Random double: ", test_random<double>(-1, 1));  // -1 to 1

  // Random forward iterable container with push_back()
  test_info("Random vector<double>: ", to_string(test_random<std::vector<double>>(5, 0, 1)));
  test_info("Random deque<int>: ", to_string(test_random<std::deque<int>>(5, -100, 100)));
  test_info("Random list<unsigned>: ", to_string(test_random<std::list<unsigned>>(5, 1, 20)));

  // Test state information
  test_info("Number of checks already done: ", test::num_checks());
  test_info("Number of fails already had: ", test::num_fails());
  test_info("Number of passes already had: ", test::num_passed());
  test_info("Number of warnings up to now: ", test::num_warnings());

  // Reset the example, as we have intentionally generated some FAILs.
  test_reset();

  // Test summary in main, no return value or statement required here.
  return;
}

// -----------------------------------------------------------------------------------------
// When using `#define WITH_MICROTEST_MAIN`, the following code is
// implicitly included in `microtest.hh` (before your test code), so
// that some global variables are available in the test run, and
// the `int main(...){}` is already there.
// Here the contents what is added:
// >>>------------------------------------------------------------------
#include <vector>
#include <string>

auto testenv_argv = std::vector<std::string>();
auto testenv_envv = std::vector<std::string>();

// Forward of the test main function in our code.
void test(const std::vector<std::string>& args);

// Application entry point with init, summary. Intentionally
// no exception catching, a logged abort exit code already
// indicates FAIL.
int main(int argc, char* argv[], char* envv[])
{
  test_initialize();
  for(size_t i = 1; (i < size_t(argc)) && (argv[i]); ++i) { testenv_argv.push_back(argv[i]); }
  for(size_t i = 0; envv[i]; ++i) { testenv_envv.push_back(envv[i]); }
  test(testenv_argv);
  return test_summary();
}

// <<<------------------------------------------------------------------
