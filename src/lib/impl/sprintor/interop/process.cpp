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

#include <cstring>
#include <string>
#include <vector>

#include <sprintor/utils/string.h>

namespace sprintor {
namespace interop {
namespace process {

void free_buffer(char ***pbuffer, int argc) {
  if (pbuffer == nullptr) {
    return;
  }
  char **buffer = *pbuffer;
  for (int i = 0; i < argc; i++) {
    free(buffer[i]);
  }
  free(buffer);
}

char **convert_to_argv(const std::vector<std::string> &argv, char ***pbuffer,
                       int *argc) {
  if (pbuffer == nullptr) {
    return nullptr;
  }
  char **buffer = (char **)malloc(sizeof(char *) * argv.size());
  for (int i = 0; i < argv.size(); i++) {
    const auto &item = argv[i];
    const auto item_size = sizeof(char) * item.size() + 1;
    char *item_buffer = (char *)malloc(item_size);
    memset(item_buffer, 0, item_size);
    strcpy(item_buffer, item.c_str());
    buffer[i] = item_buffer;
  }
  *pbuffer = buffer;
  *argc = argv.size();
  return buffer;
}

std::string convert_to_cmd(const std::vector<std::string> &argv) {
  std::string delimeter = " ";
  const auto &cmd =
      sprintor::utils::string::join(argv.begin(), argv.end(), delimeter);
  return std::move(" " + cmd);
}

std::string convert_to_args(int argc, const char **argv) {
  if (argc == 0) {
    return "";
  }
  std::string cmd = " ";
  for (int i = 0; i < argc; i++) {
    cmd.append(" " + std::string(argv[i]));
  }
  return std::move(cmd);
}

std::vector<std::string> extract_from_argv(const char **argv) {
  return std::move(std::vector<std::string>());
}

} // namespace process
} // namespace interop
} // namespace sprintor

#endif /* B76F1749_80F0_43D5_9900_87733A322CEA */
