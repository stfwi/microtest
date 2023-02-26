/**
 * @test testenv
 *
 * "Smoke test" for the test environment itself, so that it can be
 * included in project test runs without too much compile or runtime
 * losses.
 */
#include <testenv.hh>

using namespace std;


bool test_basic_macros()
{
  using namespace sw::utest;

  // "Manual-double-check" check OK statistic".
  unsigned num_ok = 0;
  const auto ok = [&num_ok](bool inc) {
    num_ok += inc ? 1 : 0;
    if(!inc) test_note("!UNEXPECTED TESTENV RESULT!");
  };

  // Unconditional pass/fail/warn
  {
    test_note("CHECK: test_pass(), test_fail(), test_warn() ...");
    test_pass("Explicit pass");
    test_fail("Explicit fail");
    test_warn("Explicit warn");
    ok(
      test::num_warnings() == 1 &&
      test::num_fails()    == 1 &&
      test::num_passed()   == 1
    );
  }

  // Test conditions for conditional execution of consecutive
  // checks on pass/fail using `if(test_expect_cond( ... )) { ... }`
  {
    test_note("CHECK: test_expect_cond(...)");
    const bool test_expect_cond_0_eq_1_has_failed = !test_expect_cond( 1 == 0 );
    ok(
      test::num_warnings() == 1 &&
      test::num_fails()    == 2 &&
      test::num_passed()   == 1 &&
      test_expect_cond_0_eq_1_has_failed
    );

    const bool test_expect_cond_1_eq_1_has_passed =  test_expect_cond( 1 == 1 );
    ok(
      test::num_warnings() == 1 &&
      test::num_fails()    == 2 &&
      test::num_passed()   == 2 &&
      test_expect_cond_1_eq_1_has_passed
    );
  }

  // Expectation of an exception / expectation that no exception is thrown.
  {
    test_note("CHECK: test_expect_except(...), test_expect_noexcept(...)");
    const auto testfn = [](){int no_exception_thrown=0; (void)no_exception_thrown;};

    test_expect_except( throw "exception for test_expect_except()" );
    ok(
      test::num_warnings() == 1 &&
      test::num_fails()    == 2 &&
      test::num_passed()   == 3
    );
    test_expect_except( testfn() );
    ok(
      test::num_warnings() == 1 &&
      test::num_fails()    == 3 &&
      test::num_passed()   == 3
    );

    test_expect_noexcept( testfn() );
    ok(
      test::num_warnings() == 1 &&
      test::num_fails()    == 3 &&
      test::num_passed()   == 4
    );

    test_expect_noexcept( throw "exception for test_expect_noexcept()" );
    ok(
      test::num_warnings() == 1 &&
      test::num_fails()    == 4 &&
      test::num_passed()   == 4
    );
  }

  // Summary note and return when fails are registered.
  {
    test_note("CHECK: test_summary() with fails");
    ok(test_summary() != 0);
  }

  // Reset test statistics
  {
    test_note("CHECK: test_reset(...)");
    test_reset();
    ok(
      test::num_warnings() == 0 &&
      test::num_fails()    == 0 &&
      test::num_passed()   == 0
    );
  }

  // Summary note and return when no fails are registered.
  {
    test_note("CHECK: test_summary() without fails");
    ok(test_summary() == 0);
  }

  // Evaluation of the test OK itself.
  test_note("Total TESTENV OK:" << num_ok);
  return num_ok == 10;
}

bool test_relational_checks()
{
  using namespace sw::utest;

  // "Manual-double-check" check OK statistic".
  unsigned num_ok = 0;
  const auto ok = [&num_ok](bool inc) {
    num_ok += inc ? 1 : 0;
    if(!inc) test_note("!UNEXPECTED TESTENV RESULT!");
  };

  int int_one = 1;
  const int const_int_one = 1;
  long long_one = 1;
  const long const_long_one = 1;
  const int const_int_two = 2;

  // EQ (==)
  {
    ok( test_expect_eq(int_one, const_int_one) );
    ok( test_expect_eq(const_long_one, const_int_one) );
    ok( test_expect_eq(const_long_one, long_one) );
    ok( test_expect_eq(string("STRING VALUE 1"), string("STRING VALUE 1")) );
    ok( test_expect_eq("STRING VALUE 2", string("STRING VALUE 2")) );
    ok( !test_expect_eq(string("STRING VALUE 1"), string("")) );
    ok( !test_expect_eq("1", string("STRING VALUE 2")) );
    ok( !test_expect_eq(const_int_one, 2) );
    ok( !test_expect_eq(42, const_long_one) );
    ok( test::num_fails() == 4 );
    ok( test::num_passed() == 5 );
    test_reset();
  }

  // NE (!=)
  {
    ok( test_expect_ne(int_one, const_int_two) );
    ok( test_expect_ne(const_long_one, const_int_two) );
    ok( test_expect_ne(const_int_two, long_one) );
    ok( test_expect_ne(string("NOT STRING VALUE 1"), string("STRING VALUE 1")) );
    ok( test_expect_ne("NOT STRING VALUE 2", string("STRING VALUE 2")) );
    ok( !test_expect_ne(string("STRING VALUE 1"), string("STRING VALUE 1")) );
    ok( !test_expect_ne("1", string("1")) );
    ok( !test_expect_ne(const_int_one, 1) );
    ok( !test_expect_ne(1, const_long_one) );
    ok( test::num_fails() == 4 );
    ok( test::num_passed() == 5 );
    test_reset();
  }

  // Evaluation of the test OK itself.
  test_note("Total TESTENV OK:" << num_ok);
  return num_ok == 22;
}

void test(const vector<string>& args)
{
  (void) args;
  test_note("+++ NOTE, THIS TEST WILL SHOW FAILS, WARNINGS, AND PASSES AS PART OF THE TEST ENVIRONMENT CHECKS +++");
  test_note("+++       ONLY THE LAST EVALUATIONS ARE RELEVANT FOR PASS/FAIL OF THE TEST-ENV CHECKS            +++");
  test_note("--------------------------------------");
  const auto basics_ok = test_basic_macros();
  test_note("--------------------------------------");
  const auto relationals_ok = test_relational_checks();
  test_note("--------------------------------------");
  test_note("-- TESTENV checks summary           --");
  test_note("--------------------------------------");
  test_expect( basics_ok );
  test_expect( relationals_ok );
}
