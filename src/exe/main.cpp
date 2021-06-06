#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

#include <boost/filesystem.hpp>

#include <sprintor/interop/process.h>

using namespace sprintor::interop::process;

inline std::vector<char> load_all_bytes(const std::string &path) {
  std::ifstream fs(path, std::ios_base::binary);
  std::vector<char> bytes;
  std::copy(std::istreambuf_iterator<char>(fs),
            std::istreambuf_iterator<char>(), std::back_insert_iterator(bytes));
  fs.close();
  return std::move(bytes);
}

int main(int argc, char *argv[]) {
#if _WIN32
  std::string exe_path = "C:\\Windows\\System32\\calc.exe";
#else
  std::string exe_path = "/bin/ls";
#endif

  const auto &exe_binary = load_all_bytes(exe_path);

  // const auto &hm =
  //     memexec("test", (void *)exe_binary.data(), exe_binary.size(), nullptr);
  // release_hmodule(hm);

  std::vector<std::string> output;
  std::vector<std::string> args{};
  const auto &r = execute_command(
      exe_binary, args, boost::filesystem::current_path().string(), output);

  for (const auto &line : output) {
    std::cout << line << std::endl;
  }

  // system("pause");

  return 0;
}
