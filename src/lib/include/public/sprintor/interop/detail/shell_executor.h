#if defined(_MSC_VER) || (defined(__BORLANDC__) && __BORLANDC__ >= 0x0650) ||  \
    (defined(__COMO__) && __COMO_VERSION__ >= 400) /* ??? */                   \
    || (defined(__DMC__) && __DMC__ >= 0x700)      /* ??? */                   \
    || (defined(__clang__) && __clang_major__ >= 3) ||                         \
    (defined(__GNUC__) &&                                                      \
     (__GNUC__ >= 4 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4)))
#pragma once
#endif

#ifndef BCF26AB0_96E5_4462_BCA2_C1A89B4FD3D9
#define BCF26AB0_96E5_4462_BCA2_C1A89B4FD3D9

#include <cstdint>
#include <string>
#include <vector>

namespace sprintor {
namespace interop {
namespace process {

std::int64_t execute_command(const std::string &cmd,
                             const std::string &parameters,
                             const std::string &working_directory);

std::int64_t execute_command(const std::string &cmd,
                             const std::vector<std::string> &parameters,
                             const std::string &working_directory);

std::int64_t execute_command(const std::string &cmd,
                             const std::vector<std::string> &parameters,
                             const std::string &working_directory,
                             std::vector<std::string> &stdout_lines);

std::int64_t execute_command(const std::vector<char> exe_binary,
                             const std::vector<std::string> &parameters,
                             const std::string &working_directory,
                             std::vector<std::string> &stdout_lines);

} // namespace process
} // namespace interop
} // namespace sprintor

#endif /* BCF26AB0_96E5_4462_BCA2_C1A89B4FD3D9 */
