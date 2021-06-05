#if defined(_MSC_VER) || (defined(__BORLANDC__) && __BORLANDC__ >= 0x0650) ||  \
    (defined(__COMO__) && __COMO_VERSION__ >= 400) /* ??? */                   \
    || (defined(__DMC__) && __DMC__ >= 0x700)      /* ??? */                   \
    || (defined(__clang__) && __clang_major__ >= 3) ||                         \
    (defined(__GNUC__) &&                                                      \
     (__GNUC__ >= 4 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4)))
#pragma once
#endif

#ifndef A5D077D2_5B8D_4E57_86E9_F3DBFC1ED99E
#define A5D077D2_5B8D_4E57_86E9_F3DBFC1ED99E

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

namespace sprintor {
namespace utils {
namespace string {

template <class T, class A> T join(const A &begin, const A &end, const T &t) {
  T result;
  for (A it = begin; it != end; it++) {
    if (!result.empty())
      result.append(t);
    result.append(*it);
  }
  return result;
}

template <typename T> std::string int_to_hex(T i, const char *prefix = "0x") {
  std::stringstream stream;
  auto width = (std::streamsize)(sizeof(T) / 8 + strlen(prefix));
  stream << prefix << std::setfill('0') << std::setw(width) << std::hex << i;
  return stream.str();
}

bool replace(std::string &str, const std::string &from, const std::string &to);

std::string replace(const std::string &str, const std::string &from,
                    const std::string &to, bool &success);

std::vector<std::string> split(const std::string &s, char seperator);

std::vector<std::string> split(const std::string &s,
                               const std::string &seperator);

std::vector<unsigned short> utf8_to_utf16(const std::string &line);

std::string utf16_to_utf8(const std::wstring &line);

std::string utf16_to_utf8(const std::vector<uint16_t> &line);

std::string utf32_to_utf8(const std::vector<uint32_t> &str);

std::vector<uint32_t> utf8_to_utf32(const std::string &str);

#if _WIN32
std::string utf8_to_local_encoding(const std::string &str);

std::string local_encoding_to_utf8(const std::string &str);
#endif

std::vector<std::uint16_t> encode_utf16_to_escaped_hex_utf16_string(
    const std::vector<std::uint16_t> &buffer);

std::vector<std::uint16_t>
decode_escaped_hex_string_to_utf16(const std::vector<std::uint16_t> &buffer);

std::string generate_guid();

} // namespace string
} // namespace utils
} // namespace sprintor

#endif /* A5D077D2_5B8D_4E57_86E9_F3DBFC1ED99E */
