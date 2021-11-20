#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#include <sprintor/interop/process.h>

namespace sprintor {
namespace interop {
namespace process {

std::int64_t memexec(const std::string &file_name,
                     const std::vector<char> bin_buffer,
                     const std::vector<std::string> &argv) {
  return memexec(file_name, (void *)bin_buffer.data(), bin_buffer.size(), argv);
}

} // namespace process
} // namespace interop
} // namespace sprintor