#if defined(_MSC_VER) || (defined(__BORLANDC__) && __BORLANDC__ >= 0x0650) ||  \
    (defined(__COMO__) && __COMO_VERSION__ >= 400) /* ??? */                   \
    || (defined(__DMC__) && __DMC__ >= 0x700)      /* ??? */                   \
    || (defined(__clang__) && __clang_major__ >= 3) ||                         \
    (defined(__GNUC__) &&                                                      \
     (__GNUC__ >= 4 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4)))
#pragma once
#endif

#ifndef A1E727F0_970E_44DE_9EF6_12EBAAD3003C
#define A1E727F0_970E_44DE_9EF6_12EBAAD3003C

#include <string>

namespace sprintor {
namespace interop {
namespace file_system {

std::vector<char> load_all_bytes(const std::string &path);

}
} // namespace interop
} // namespace sprintor

#endif /* A1E727F0_970E_44DE_9EF6_12EBAAD3003C */
