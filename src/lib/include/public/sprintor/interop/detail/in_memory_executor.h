#if defined(_MSC_VER) || (defined(__BORLANDC__) && __BORLANDC__ >= 0x0650) ||  \
    (defined(__COMO__) && __COMO_VERSION__ >= 400) /* ??? */                   \
    || (defined(__DMC__) && __DMC__ >= 0x700)      /* ??? */                   \
    || (defined(__clang__) && __clang_major__ >= 3) ||                         \
    (defined(__GNUC__) &&                                                      \
     (__GNUC__ >= 4 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4)))
#pragma once
#endif

#ifndef F689341E_65DA_4BDB_B0FA_1E2ACD86763C
#define F689341E_65DA_4BDB_B0FA_1E2ACD86763C

#include <cstdint>
#include <string>
#include <vector>

namespace sprintor {
namespace interop {
namespace process {

std::int64_t memexec(const std::string &file_name, void *exe,
                     std::size_t exe_size, int argc, char **argv);

std::int64_t memexec(const std::string &file_name, void *exe,
                     std::size_t exe_size,
                     const std::vector<std::string> &argv);

std::int64_t memexec(const std::string &file_name,
                     const std::vector<char> bin_buffer,
                     const std::vector<std::string> &argv);

} // namespace process
} // namespace interop
} // namespace sprintor

#endif /* F689341E_65DA_4BDB_B0FA_1E2ACD86763C */
