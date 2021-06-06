#include <cstdint>
#include <fstream>
#include <string>
#include <vector>

#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <boost/process.hpp>
#include <boost/process/async.hpp>

#include <sprintor/interop/detail/shell_executor.h>
#include <sprintor/utils/string.h>

namespace bp = boost::process;
namespace bfs = boost::filesystem;
using namespace sprintor::utils::string;

namespace sprintor {
namespace interop {
namespace process {

struct process_result {
  std::string out;
  std::string err;
  int exit_code;
};

inline std::string
write_all_bytes_to_temp_file(const std::vector<char> &binary) {
  bfs::path tempPath = bfs::temp_directory_path() / bfs::unique_path();
  const auto &tempPathStr = tempPath.string();
  std::ofstream fs(tempPathStr, std::ios_base::binary);
  fs.write(binary.data(), binary.size());
  fs.close();
  bfs::permissions(tempPath, bfs::add_perms | bfs::owner_write |
                                 bfs::group_write | bfs::others_write |
                                 bfs::group_exe | bfs::owner_exe |
                                 bfs::others_exe);
  return tempPathStr;
}

inline process_result execute_process_async(std::string exe,
                                            std::vector<std::string> args,
                                            boost::asio::io_service &io) {
  std::future<std::string> out, err;
  bp::group group;

  bp::child child(exe, args, bp::std_in.close(), bp::std_out > out,
                  bp::std_err > err, io, group);

  child.wait();

  return {out.get(), err.get(), child.exit_code()};
}

std::int64_t execute_command(const std::string &cmd,
                             const std::vector<std::string> &parameters,
                             const std::string &working_directory,
                             std::vector<std::string> &stdout_lines) {
  bp::ipstream out;
  bp::child c(cmd, parameters, bp::std_out > out);

  for (std::string line; c.running() && std::getline(out, line);) {
    stdout_lines.push_back(line);
  }

  c.wait();

  return 0;
}

std::int64_t execute_command(const std::vector<char> exe_binary,
                             const std::vector<std::string> &parameters,
                             const std::string &working_directory,
                             std::vector<std::string> &stdout_lines) {
  const auto &path = write_all_bytes_to_temp_file(exe_binary);

  boost::asio::io_service io;

  auto work = boost::asio::make_work_guard(io);
  std::thread io_thread([&io] { io.run(); });

  auto pstree = execute_process_async(path, parameters, io);
  stdout_lines.push_back(pstree.out);

  work.reset();
  io_thread.join();

  bfs::remove(path);

  return pstree.exit_code;
}

} // namespace process
} // namespace interop
} // namespace sprintor