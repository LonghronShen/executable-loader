#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace sprintor {
namespace interop {
namespace file_system {

std::vector<char> load_all_bytes(const std::string &path) {
  std::ifstream fs(path, std::ios_base::binary);
std::vector<char> bytes;
  std::copy(std::istreambuf_iterator<char>(fs),
            std::istreambuf_iterator<char>(), std::back_insert_iterator(bytes));
  fs.close();
  return std::move(bytes);
}

} // namespace file_system
} // namespace interop
} // namespace sprintor