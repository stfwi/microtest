/**
 * @test tmpfile
 *
 * Checks the `tmp_file` creation and deletion.
 * (Compat also with <c++17, so no filesystem is used, paths
 * are string).
 */
#define WITH_MICROTEST_TMPFILE
#define WITH_MICROTEST_TMPDIR
#include <testenv.hh>
#include <algorithm>
#include <iomanip>
#include <vector>
#include <cmath>
#include <array>

using namespace std;

bool isfile(const string& path)
{
#ifdef _MSC_VER
  const auto attr = ::GetFileAttributesA(path.c_str());
  return (attr != INVALID_FILE_ATTRIBUTES) && (!(attr & FILE_ATTRIBUTE_DIRECTORY));
#else
  struct ::stat st;
  return (::stat(path.c_str(), &st) == 0) && (S_ISREG(st.st_mode));
#endif
}

bool isdir(const string& path)
{
#ifdef _MSC_VER
  const auto attr = ::GetFileAttributesA(path.c_str());
  return (attr != INVALID_FILE_ATTRIBUTES) && (attr & FILE_ATTRIBUTE_DIRECTORY);
#else
  struct ::stat st;
  return (::stat(path.c_str(), &st) == 0) && (S_ISDIR(st.st_mode));
#endif
}

void test_tmpfile_handling()
{
  using namespace ::sw::utest;
  auto tmpfile1_path = string();
  {
    test_info("Constructing tmp_file in scope ...");
    const auto tmpfile1 = test_make_tmpfile();
    tmpfile1_path = tmpfile1.path();
    test_info("tmpfile1.path() = '", tmpfile1_path, "'");
    test_expect(isfile(tmpfile1_path));
  }
  test_info("Checking if tmp_file deleted after falling out of scope ...");
  test_expect(!isfile(tmpfile1_path));
}

void test_tmpdir_handling()
{
  using namespace ::sw::utest;
  auto dir_path = string();
  {
    test_info("Constructing tmp_file in scope ...");
    const auto dir = test_make_tmpdir();
    dir_path = dir.path();
    test_info("tmpfile1.path() = '", dir_path, "'");
    test_expect(isdir(dir_path));
  }
  test_info("Checking if tmp_file deleted after falling out of scope ...");
  test_expect(!isdir(dir_path));
}

void test(const vector<string>& args)
{
  using namespace ::sw::utest;
  (void)args;
  test_tmpfile_handling();
  test_tmpdir_handling();
}
