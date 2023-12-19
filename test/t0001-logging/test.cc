/**
 * @test logging
 *
 * Checks the api and the logging output by diverting
 * the normal output stream from stdout to string streams.
 * Also settings like forced ANSI/no-ANSI, or PASS log
 * omission is checked here.
 */
#include <testenv.hh>
#include <algorithm>

using namespace std;

// Guard to prevent stream overrides (which is normally not done)
// falling out of scope.
struct teststream_restore
{
  teststream_restore() noexcept = default;

  ~teststream_restore() noexcept { ::sw::utest::test::stream(std::cout); }
};

void test(const vector<string>& args)
{
  using namespace sw::utest;
  (void)args;
  auto testenv_fails = std::string();

  const auto stripped = [](std::string&& s) {
    s.erase(std::remove_if(s.begin(), s.end(), [](char c) { return c < ' '; }), s.end());
    return s;
  };

  const auto text_check_noansi = [&]() -> void {
    const auto was_ansi = test::ansi_colors();
    const auto restore = teststream_restore();
    auto os = std::stringstream();
    test::ansi_colors(false);
    test::stream(os);
    test::pass();
    test::fail();
    test::warning("FILENAME", 1, "warning-message");
    test::pass("FILENAME", 2, "pass-message");
    test::fail("FILENAME", 3, "fail-message");
    const auto exit_code = test::summary();
    test::stream(cout);
    test::ansi_colors(was_ansi);
    const auto num_pass = test::num_passed();
    const auto num_fail = test::num_fails();
    const auto num_warn = test::num_warnings();
    test::reset();

    const auto s = stripped(os.str());
    test_info("text_check_noansi: ", s);
    test_expect(
      s
      == "[warn] [@FILENAME:1] warning-message[pass] [@FILENAME:2] pass-message[fail] [@FILENAME:3] fail-message[FAIL] "
         "2 of 4 checks failed, 1 warnings.");
    test_expect(exit_code > 0);
    test_expect(num_pass == 2);
    test_expect(num_fail == 2);
    test_expect(num_warn == 1);
    if(test::num_fails() > 0) { testenv_fails += "text_check_noansi failed. "; }
    test::reset();
  };

  const auto text_check_ansi = [&]() -> void {
    const auto was_ansi = test::ansi_colors();
    const auto restore = teststream_restore();
    auto os = std::stringstream();
    test::ansi_colors(true);
    test::stream(os);
    test::warning("FILE", 1, "warn-msg");
    test::pass("FILE", 2, "pass-msg");
    test::fail("FILE", 3, "fail-msg");
    test::stream(cout);
    test::ansi_colors(was_ansi);
    test::reset();

    const auto s = stripped(os.str());
    test_info("text_check_ansi: ", s);
    test_expect(
      s
      == "[0;33m[warn][0m [0;36m[@FILE:1][0m warn-msg[0m[0;32m[pass][0m [0;36m[@FILE:2][0m pass-msg[0m[0;31m[fail][0m "
         "[0;36m[@FILE:3][0m fail-msg[0m");
    if(test::num_fails() > 0) { testenv_fails += "text_check_ansi failed. "; }
    test::reset();
  };

  const auto text_check_omit_combined = [&]() -> void {
    const auto was_ansi = test::ansi_colors();
    const auto was_omit = test::omit_pass_log();
    const auto restore = teststream_restore();
    auto os = std::stringstream();
    test::stream(os);

    test::ansi_colors(true);
    test::warning("F", 1, "W");
    test::pass("F", 2, "P");
    test::fail("F", 3, "F");
    test::reset();
    test::ansi_colors(false);
    test::warning("F", 1, "W");
    test::pass("F", 2, "P");
    test::fail("F", 3, "F");
    test::reset();

    os.str("");
    test::omit_pass_log(true);
    test::warning("F", 1, "W");
    test::commit(true, "F", 2, "P");
    test::commit(true);
    test::commit(false);
    test::pass("F", 2, "P");
    test::fail("F", 3, "F");
    test::pass("F", 4, "P");
    test::pass("F", 5, "P");
    test::pass("F", 6, "P");
    const auto num_unlogged_passes = test::num_passed();
    test::reset();

    test::ansi_colors(was_ansi);
    test::omit_pass_log(was_omit);
    test::stream(cout);
    test::reset();

    const auto s = stripped(os.str());
    test_info("text_check_omit_combined: ", s);
    test_expect(s == "[warn] [@F:1] W[fail] [@F:3] F");  // <- no [pass] shall occur, but the others
    test_expect(num_unlogged_passes == 6);
    if(test::num_fails() > 0) { testenv_fails += "text_check_omit_combined failed. "; }
    test::reset();
  };

  const auto text_check_summary_allpass = [&]() -> void {
    const auto was_ansi = test::ansi_colors();
    const auto was_omit = test::omit_pass_log();
    const auto restore = teststream_restore();
    auto os = std::stringstream();
    test::stream(os);
    test::pass("F", 2, "P");
    test_summary();
    test::ansi_colors(was_ansi);
    test::omit_pass_log(was_omit);
    test::stream(cout);
    test::reset();
    const auto s = stripped(os.str());
    test_info("text_check_summary_allpass: ", s);
    test_expect(s == "[pass] [@F:2] P[PASS] All 1 checks passed, 0 warnings.");
    if(test::num_fails() > 0) { testenv_fails += "text_check_summary_allpass failed. "; }
    test::reset();
  };

  const auto text_check_summary_pass_warnings = [&]() -> void {
    const auto was_ansi = test::ansi_colors();
    const auto was_omit = test::omit_pass_log();
    const auto restore = teststream_restore();
    auto os = std::stringstream();
    test::stream(os);
    test::warning("F", 1, "W");
    test::pass("F", 2, "P");
    test_summary();
    test::ansi_colors(was_ansi);
    test::omit_pass_log(was_omit);
    test::stream(cout);
    test::reset();
    const auto s = stripped(os.str());
    test_info("text_check_summary_pass_warnings: ", s);
    test_expect(s == "[warn] [@F:1] W[pass] [@F:2] P[PASS] All 1 checks passed, 1 warnings.");
    if(test::num_fails() > 0) { testenv_fails += "text_check_summary_pass_warnings failed. "; }
    test::reset();
  };

  const auto text_check_summary_no_checks = [&]() -> void {
    const auto was_ansi = test::ansi_colors();
    const auto restore = teststream_restore();
    auto os = std::stringstream();
    test::stream(os);
    test_summary();
    test::ansi_colors(was_ansi);
    test::stream(cout);
    test::reset();
    const auto s = stripped(os.str());
    test_info("text_check_summary_no_checks: ", s);
    test_expect(s == "[DONE] No checks");
    if(test::num_fails() > 0) { testenv_fails += "text_check_summary_no_checks failed. "; }
    test::reset();
  };

  // Run all.
  text_check_noansi();
  text_check_ansi();
  text_check_omit_combined();
  text_check_summary_allpass();
  text_check_summary_pass_warnings();
  text_check_summary_no_checks();
  test_info("---------------------------------------------");
  test_info("Check fails: ", testenv_fails);
  test_expect(testenv_fails.empty());
}
