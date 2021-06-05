#include <cstdint>
#include <string>

#include <crossguid/guid.hpp>
#include <utf8.h>

#include <sprintor/utils/string.h>

namespace sprintor {
namespace utils {
namespace string {

bool replace(std::string &str, const std::string &from, const std::string &to) {
  size_t start_pos = str.find(from);
  if (start_pos == std::string::npos)
    return false;
  str.replace(start_pos, from.length(), to);
  return true;
}

std::string replace(const std::string &str, const std::string &from,
                    const std::string &to, bool &success) {
  std::string copy = str;
  success = replace(copy, from, to);
  return std::move(copy);
}

std::vector<uint16_t> utf8_to_utf16(const std::string &line) {
  std::vector<uint16_t> utf16line;
  utf8::utf8to16(line.begin(), line.end(), std::back_inserter(utf16line));
  utf16line.push_back(0);
  return std::move(utf16line);
}

std::string utf16_to_utf8(const std::wstring &line) {
  std::string utf8line;
  wchar_t *start = (wchar_t *)line.data();
  wchar_t *end = start + line.length();
  utf8::utf16to8(start, end, std::back_inserter(utf8line));
  return std::move(utf8line);
}

std::string utf16_to_utf8(const std::vector<uint16_t> &line) {
  std::string utf8line;
  utf8::utf16to8(line.begin(), line.end(), std::back_inserter(utf8line));
  return std::move(utf8line);
}

std::string utf32_to_utf8(const std::vector<uint32_t> &str) {
  std::string line;
  utf8::utf32to8(str.begin(), str.end(), back_inserter(line));
  return std::move(line);
}

std::vector<uint32_t> utf8_to_utf32(const std::string &str) {
  std::vector<uint32_t> line;
  utf8::utf8to32(str.begin(), str.end(), std::back_inserter(line));
  return std::move(line);
}

std::vector<std::uint16_t> encode_utf16_to_escaped_hex_utf16_string(
    const std::vector<std::uint16_t> &buffer) {
  std::string line;
  for (const auto &c : buffer) {
    line += int_to_hex(c, "\\u");
  }
  return std::move(utf8_to_utf16(line));
}

std::vector<std::uint16_t>
decode_escaped_hex_string_to_utf16(const std::vector<std::uint16_t> &buffer) {
  std::vector<std::uint16_t> line;
  const auto &u8_encoded_string = utf16_to_utf8(buffer);
  const auto &items = split(u8_encoded_string, "\\u");
  for (const auto &c : items) {
    std::string s = "0x" + c;
    auto x = std::stoul(s, nullptr, 16);
    line.push_back((std::uint16_t)x);
  }
  return std::move(line);
}

template <typename T>
std::vector<std::string> _split(const std::string &s, const T &seperator) {
  std::vector<std::string> output;
  std::string::size_type prev_pos = 0, pos = 0;

  while ((pos = s.find(seperator, pos)) != std::string::npos) {
    std::string substring(s.substr(prev_pos, pos - prev_pos));
    output.push_back(substring);
    prev_pos = ++pos;
  }

  output.push_back(s.substr(prev_pos, pos - prev_pos)); // Last word

  return output;
}

std::vector<std::string> split(const std::string &s, char seperator) {
  return _split(s, seperator);
}

std::vector<std::string> split(const std::string &s,
                               const std::string &seperator) {
  return _split(s, seperator);
}

std::string generate_guid() {
  const auto &guid = xg::newGuid();
  return (std::string)guid;
}

} // namespace string
} // namespace utils
} // namespace sprintor
