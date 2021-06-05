#if defined(_MSC_VER) || (defined(__BORLANDC__) && __BORLANDC__ >= 0x0650) ||  \
    (defined(__COMO__) && __COMO_VERSION__ >= 400) /* ??? */                   \
    || (defined(__DMC__) && __DMC__ >= 0x700)      /* ??? */                   \
    || (defined(__clang__) && __clang_major__ >= 3) ||                         \
    (defined(__GNUC__) &&                                                      \
     (__GNUC__ >= 4 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4)))
#pragma once
#endif

#ifndef B76F1749_80F0_43D5_9900_87733A322CEA
#define B76F1749_80F0_43D5_9900_87733A322CEA

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>


namespace sprintor {
namespace interop {
namespace process {

void free_buffer(char ***buffer, int argc);

void release_hmodule(std::uint64_t hmodule);

std::vector<std::string> extract_from_argv(const char **argv);

std::string convert_to_args(int argc, const char **argv);

char **convert_to_argv(const std::vector<std::string> &argv, char ***buffer,
                       int *argc);

std::string convert_to_cmd(const std::vector<std::string> &argv);

std::uint64_t memexec(const std::string &file_name, void *exe,
                      std::size_t exe_size, const char **argv);

std::uint64_t memexec(const std::string &file_name, void *exe,
                      std::size_t exe_size,
                      const std::vector<std::string> &argv);

} // namespace process
} // namespace interop
} // namespace sprintor

#endif /* B76F1749_80F0_43D5_9900_87733A322CEA */
