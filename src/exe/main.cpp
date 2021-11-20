#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

#include <boost/filesystem.hpp>

#include <sprintor/interop/process.h>

#define INSTALL_IN_MEMORY 1

using namespace sprintor::interop::process;

std::vector<char> load_all_bytes(const std::string &path) {
  std::ifstream fs(path, std::ios_base::binary);
  std::vector<char> bytes;
  std::copy(std::istreambuf_iterator<char>(fs),
            std::istreambuf_iterator<char>(), std::back_insert_iterator(bytes));
  fs.close();
  return std::move(bytes);
}

int main(int argc, char *argv[]) {
#if _WIN32
  std::string exe_path = "./executable-loader-test-1.exe";
#else
  std::string exe_path = "./executable-loader-test-1";
#endif

  std::vector<std::string> args;

  if (argc > 1) {
    exe_path = argv[1];
    std::copy(argv + 2, argv + argc, std::back_inserter(args));
  }

  const auto &exe_binary = load_all_bytes(exe_path);

#if INSTALL_IN_MEMORY
  args.insert(args.begin(), "test");
  const auto &hm = memexec("test", exe_binary, args);
  release_hmodule(hm);
#else
  std::vector<std::string> output;
  const auto &r = execute_command(
      exe_binary, args, boost::filesystem::current_path().string(), output);

  for (const auto &line : output) {
    std::cout << line << std::endl;
  }
#endif

#if defined(_WIN32)
  system("pause");
#endif

  return 0;
}
