#if __linux__

#include <cstdio>  // perror
#include <cstdlib> // mkostemp
#include <cstring> // size_t
#include <string>

#include <fcntl.h>
#include <sys/stat.h> // O_WRONLY
#include <unistd.h>   // read

#include <sprintor/interop/process.h>

namespace sprintor {
namespace interop {
namespace process {

void release_hmodule(std::uint64_t hmodule) { return; }

std::uint64_t memexec(const std::string &file_name, void *exe,
                      std::size_t exe_size,
                      const std::vector<std::string> &argv) {
  char **args_array;
  int argc;
  if (convert_to_argv(argv, &args_array, &argc)) {
    auto hmodule = memexec(file_name, exe, exe_size, args_array);
    free_buffer(&args_array, argc);
    return (std::uint64_t)hmodule;
  }
  return -1;
}

std::uint64_t memexec(const std::string &file_name, void *exe,
                      std::size_t exe_size, const char **argv) {
  /* random temporary file name in /tmp */
  const auto &path = "/tmp/" + file_name;
  char *name = (char *)path.c_str();

  /* creates temporary file, returns writeable file descriptor */
  int fd_wr = mkostemp(name, O_WRONLY);
  /* makes file executable and readonly */
  chmod(name, S_IRUSR | S_IXUSR);
  /* creates read-only file descriptor before deleting the file */
  int fd_ro = open(name, O_RDONLY);
  /* removes file from file system, kernel buffers content in memory until all
   * fd closed */
  unlink(name);
  /* writes executable to file */
  write(fd_wr, exe, exe_size);
  /* fexecve will not work as long as there in a open writeable file descriptor
   */
  close(fd_wr);
  char *const newenviron[] = {NULL};
  /* -fpermissive */
  fexecve(fd_ro, (char *const *)argv, newenviron);
  perror("failed");

  return 0;
}

} // namespace process
} // namespace interop
} // namespace sprintor

#endif