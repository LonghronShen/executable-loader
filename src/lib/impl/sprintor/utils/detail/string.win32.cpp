#if defined(_WIN32) || defined(_WIN64)

#include <cstdint>
#include <string>

#include <Windows.h>

#include <sprintor/utils/string.h>

namespace sprintor {
namespace utils {
namespace string {

std::string utf8_to_local_encoding(const std::string &str) {
  int len = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
  PWSTR wszGBK = new WCHAR[len + 1];
  memset(wszGBK, 0, len * 2 + 2);
  MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, wszGBK, len);
  len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
  char *szGBK = new char[len + 1];
  memset(szGBK, 0, len + 1);
  WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
  std::string strTemp = szGBK;
  if (wszGBK) {
    delete[] wszGBK;
  }
  if (szGBK) {
    delete[] szGBK;
  }
  return strTemp;
}

std::string local_encoding_to_utf8(const std::string &str) {
  int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
  wchar_t *wstr = new wchar_t[len + 1];
  memset(wstr, 0, len + 1);
  MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, wstr, len);
  len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
  char *szUtf8 = new char[len + 1];
  memset(szUtf8, 0, len + 1);
  WideCharToMultiByte(CP_UTF8, 0, wstr, -1, szUtf8, len, NULL, NULL);
  std::string strTemp = szUtf8;
  if (wstr) {
    delete[] wstr;
  }
  if (szUtf8) {
    delete[] szUtf8;
  }
  return strTemp;
}

} // namespace string
} // namespace utils
} // namespace sprintor

#endif