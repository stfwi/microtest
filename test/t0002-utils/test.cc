/**
 * @test utils
 *
 * Checks that the utility wrappers (around normal STL or C)
 * work. This does not test the STL functionality itself
 * (like the random engine).
 */
#include <testenv.hh>
#include <algorithm>
#include <iomanip>
#include <vector>
#include <cmath>
#include <array>

using namespace std;

template<typename T1, typename T2, typename T3>
bool in_range(T1 v, T2 min, T3 max)
{
  // -> c++14 in the function, broke c++11.
  // const auto in_range = [](auto v, auto min, auto max){
  //   using val_t = typename ::decay<decltype(v)>::type;
  //   return (v>=val_t(min)) && (v<=val_t(max));
  // };
  // const auto all_in_range = [&in_range](auto c, auto min, auto max){
  //   for(const auto v:c) { if(!in_range(v, min, max)) return false; } return true;
  // };
  using val_t = typename ::decay<decltype(v)>::type;
  return (v >= val_t(min)) && (v <= val_t(max));
}

template<typename C, typename T2, typename T3>
bool all_in_range(const C& c, T2 min, T3 max)
{
  for(const auto v: c) {
    if(!in_range(v, min, max)) { return false; }
  }
  return true;
}

void test(const vector<string>& args)
{
  using namespace ::sw::utest;
  (void)args;

  // Arithmetic auxilary functions
  {
    // Euler constant
    static const auto f32_e = exp(float(1));
    static const auto f64_e = exp(double(1));
    static const auto f80_e = exp((long double)(1));  // long double >=80 bit
    test_info("scexpr_f32_e==const_f32_e==f32_e = ", to_string(f32_e, 10));
    test_info("scexpr_f64_e==const_f64_e==f64_e = ", to_string(f64_e, 20));
    test_info("scexpr_f80_e==const_f80_e==f80_e = ", to_string(f80_e, 25));
    // Accuracy must be converted up or down to the input value type without warnings.
    test_expect_eq(round(f32_e, double(1e0)), 3);
    test_expect_eq(round(f64_e, double(1e0)), 3);
    test_expect_eq(round(f80_e, double(1e0)), 3);
    test_expect_eq(round(f32_e, float(1e0)), 3);
    test_expect_eq(round(f64_e, float(1e0)), 3);
    test_expect_eq(round(f80_e, float(1e0)), 3);
    test_expect_eq(round(f32_e, (long double)(1e0)), 3);
    test_expect_eq(round(f64_e, (long double)(1e0)), 3);
    test_expect_eq(round(f80_e, (long double)(1e0)), 3);
    // Must be 0 < accuracy <= 1, quick check how the calculation works out:
    test_expect_eq(round(f32_e, double(0e+00)), f32_e);
    test_expect_eq(round(f32_e, double(1e+01)), 0);
    static constexpr auto eps = std::numeric_limits<double>::epsilon() * 3;
    test_expect(abs(round(f64_e, double(1e-01)) - 2.7) < eps);
    test_expect(abs(round(f64_e, double(1e-02)) - 2.72) < eps);
    test_expect(abs(round(f64_e, double(1e-03)) - 2.718) < eps);
    test_expect(abs(round(f64_e, double(1e-04)) - 2.7183) < eps);
    test_expect(abs(round(f64_e, double(1e-05)) - 2.71828) < eps);
    test_expect(abs(round(f64_e, double(1e-06)) - 2.718282) < eps);
    test_expect(abs(round(f64_e, double(1e-07)) - 2.7182818) < eps);
    test_expect(abs(round(f64_e, double(1e-08)) - 2.71828183) < eps);
    test_expect(abs(round(f64_e, double(1e-09)) - 2.718281828) < eps);
    test_expect(abs(round(f64_e, double(1e-10)) - 2.7182818285) < eps);
    test_expect(abs(round(f64_e, double(1e-11)) - 2.71828182846) < eps);
    test_expect(abs(round(f64_e, double(1e-12)) - 2.718281828459) < eps);
    test_expect(abs(round(f64_e, double(1e-13)) - 2.7182818284590) < eps);
    test_expect(abs(round(f64_e, double(1e-14)) - 2.71828182845905) < eps);
    test_expect(abs(round(f64_e, double(1e-15)) - 2.718281828459045) < eps);
  }

  // is_windows()
  {
    // for(const auto& e:testenv_envv) test_info(e);
    const bool env_contains_windir =
      testenv_envv.end() != std::find_if(testenv_envv.begin(), testenv_envv.end(), [](const std::string& s) {
        return s.find("WINDIR=") == 0;
      });
    test_expect_eq(buildinfo::is_windows(), env_contains_windir);
  }

  // random()
  {
    {
      test_info("Random number generation: Type compat checks ...");
      test_expect_noexcept((void)test_random<char>());
      test_expect_noexcept((void)test_random<short>());
      test_expect_noexcept((void)test_random<int>());
      test_expect_noexcept((void)test_random<long>());
      test_expect_noexcept((void)test_random<long long>());
      test_expect_noexcept((void)test_random<float>());
      test_expect_noexcept((void)test_random<double>());
      test_expect_noexcept((void)test_random<long double>());
      test_expect_noexcept((void)test_random<unsigned char>());
      test_expect_noexcept((void)test_random<unsigned short>());
      test_expect_noexcept((void)test_random<unsigned int>());
      test_expect_noexcept((void)test_random<unsigned long>());
      test_expect_noexcept((void)test_random<unsigned long long>());
    }

    {
      test_info("Random number generation: 0..max type compat checks ...");
      constexpr auto max = 10;
      test_expect(in_range(test_random<char>(max), 0, max));
      test_expect(in_range(test_random<short>(max), 0, max));
      test_expect(in_range(test_random<int>(max), 0, max));
      test_expect(in_range(test_random<long>(max), 0, max));
      test_expect(in_range(test_random<long long>(max), 0, max));
      test_expect(in_range(test_random<float>(max), 0, max));
      test_expect(in_range(test_random<double>(max), 0, max));
      test_expect(in_range(test_random<long double>(max), 0, max));
      test_expect(in_range(test_random<unsigned char>(max), 0, max));
      test_expect(in_range(test_random<unsigned short>(max), 0, max));
      test_expect(in_range(test_random<unsigned int>(max), 0, max));
      test_expect(in_range(test_random<unsigned long>(max), 0, max));
      test_expect(in_range(test_random<unsigned long long>(max), 0, max));
    }
    {
      test_info("Random number generation: min..max type compat checks ...");
      constexpr auto min = 5;
      constexpr auto max = 10;
      test_expect(in_range(test_random<char>(min, max), min, max));
      test_expect(in_range(test_random<short>(min, max), min, max));
      test_expect(in_range(test_random<int>(min, max), min, max));
      test_expect(in_range(test_random<long>(min, max), min, max));
      test_expect(in_range(test_random<long long>(min, max), min, max));
      test_expect(in_range(test_random<float>(min, max), min, max));
      test_expect(in_range(test_random<double>(min, max), min, max));
      test_expect(in_range(test_random<long double>(min, max), min, max));
      test_expect(in_range(test_random<unsigned char>(min, max), min, max));
      test_expect(in_range(test_random<unsigned short>(min, max), min, max));
      test_expect(in_range(test_random<unsigned int>(min, max), min, max));
      test_expect(in_range(test_random<unsigned long>(min, max), min, max));
      test_expect(in_range(test_random<unsigned long long>(min, max), min, max));
    }
    {
      test_info("Random number generation: min..max type compat checks ...");
      constexpr auto min = 5;
      constexpr auto max = 10;
      test_expect(in_range(test_random<char>(min, max), min, max));
      test_expect(in_range(test_random<short>(min, max), min, max));
      test_expect(in_range(test_random<int>(min, max), min, max));
      test_expect(in_range(test_random<long>(min, max), min, max));
      test_expect(in_range(test_random<long long>(min, max), min, max));
      test_expect(in_range(test_random<float>(min, max), min, max));
      test_expect(in_range(test_random<double>(min, max), min, max));
      test_expect(in_range(test_random<long double>(min, max), min, max));
      test_expect(in_range(test_random<unsigned char>(min, max), min, max));
      test_expect(in_range(test_random<unsigned short>(min, max), min, max));
      test_expect(in_range(test_random<unsigned int>(min, max), min, max));
      test_expect(in_range(test_random<unsigned long>(min, max), min, max));
      test_expect(in_range(test_random<unsigned long long>(min, max), min, max));
    }
    {
      test_info("Random number generation: Container min..max type compat checks ...");
      constexpr size_t n = 20;
      constexpr auto min = 5;
      constexpr auto max = 10;
      test_expect(all_in_range(test_random<vector<char>>(n, min, max), min, max));
      test_expect(all_in_range(test_random<vector<short>>(n, min, max), min, max));
      test_expect(all_in_range(test_random<vector<int>>(n, min, max), min, max));
      test_expect(all_in_range(test_random<vector<long>>(n, min, max), min, max));
      test_expect(all_in_range(test_random<vector<long long>>(n, min, max), min, max));
      test_expect(all_in_range(test_random<vector<float>>(n, min, max), min, max));
      test_expect(all_in_range(test_random<vector<double>>(n, min, max), min, max));
      test_expect(all_in_range(test_random<vector<long double>>(n, min, max), min, max));
      test_expect(all_in_range(test_random<vector<unsigned char>>(n, min, max), min, max));
      test_expect(all_in_range(test_random<vector<unsigned short>>(n, min, max), min, max));
      test_expect(all_in_range(test_random<vector<unsigned int>>(n, min, max), min, max));
      test_expect(all_in_range(test_random<vector<unsigned long>>(n, min, max), min, max));
      test_expect(all_in_range(test_random<vector<unsigned long long>>(n, min, max), min, max));
    }
    {
      test_info("Random number generation: Container min<type>..min<type> compat checks ...");
      constexpr size_t n = 20;
      test_expect_noexcept((void)test_random<vector<char>>(n));
      test_expect_noexcept((void)test_random<vector<short>>(n));
      test_expect_noexcept((void)test_random<vector<int>>(n));
      test_expect_noexcept((void)test_random<vector<long>>(n));
      test_expect_noexcept((void)test_random<vector<long long>>(n));
      test_expect_noexcept((void)test_random<vector<float>>(n));
      test_expect_noexcept((void)test_random<vector<double>>(n));
      test_expect_noexcept((void)test_random<vector<long double>>(n));
      test_expect_noexcept((void)test_random<vector<unsigned char>>(n));
      test_expect_noexcept((void)test_random<vector<unsigned short>>(n));
      test_expect_noexcept((void)test_random<vector<unsigned int>>(n));
      test_expect_noexcept((void)test_random<vector<unsigned long>>(n));
      test_expect_noexcept((void)test_random<vector<unsigned long long>>(n));
    }
  }

  // Sequences
  {
    const auto seqv_long_1_100 = sequence_vector<long, 1>(100);
    test_expect(seqv_long_1_100.size() == 1);
    test_expect(seqv_long_1_100.front() == 100);
    const auto seqv_int_10_0 = sequence_vector<int, 10>(0);
    test_expect(seqv_int_10_0.size() == 10);
    test_expect(seqv_int_10_0.front() == 0);
    test_expect(seqv_int_10_0.back() == 9);
    const auto seqv_double_2_42 = sequence_vector<double, 2>(42);
    test_expect(seqv_double_2_42.size() == 2);
    test_expect(seqv_double_2_42.front() == 42);
    test_expect(seqv_double_2_42.back() == 43);
    const auto seqa_long_1_100 = sequence_array<long, 1>(100);
    test_expect(seqa_long_1_100.max_size() == 1);
    test_expect(seqa_long_1_100.front() == 100);
    const auto seqa_int_10_0 = sequence_array<int, 10>(0);
    test_expect(seqa_int_10_0.max_size() == 10);
    test_expect(seqa_int_10_0.front() == 0);
    test_expect(seqa_int_10_0.back() == 9);
    const auto seqa_double_2_42 = sequence_array<double, 2>(42);
    test_expect(seqa_double_2_42.max_size() == 2);
    test_expect(seqa_double_2_42.front() == 42);
    test_expect(seqa_double_2_42.back() == 43);
    test_expect(test_sequence_vector<char, 1>(' ').size() == 1);
    test_expect(test_sequence_array<char, 10>('\0').size() == 10);
  }
}
