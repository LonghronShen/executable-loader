#if defined(_WIN32)

#include <cstdint>
#include <string>
#include <vector>

#include <Windows.h>

#include <ShellAPI.h>
#include <VersionHelpers.h>


#include <sprintor/interop/detail/shell_executor.h>
#include <sprintor/utils/string.h>

using namespace sprintor::utils::string;

namespace sprintor {
namespace interop {
namespace process {

inline bool IsWindowsXP() {
  return IsWindowsXPOrGreater() && !IsWindowsVistaOrGreater();
}

inline void GetAllWindowsFromProcessID(DWORD dwProcessID,
                                       std::vector<HWND> &vhWnds) {
  // find all hWnds (vhWnds) associated with a process id (dwProcessID)
  HWND hCurWnd = nullptr;
  do {
    hCurWnd = FindWindowEx(nullptr, hCurWnd, nullptr, nullptr);
    DWORD checkProcessID = 0;
    GetWindowThreadProcessId(hCurWnd, &checkProcessID);
    if (checkProcessID == dwProcessID) {
      vhWnds.push_back(hCurWnd); // add the found hCurWnd to the vector
                                 // wprintf(L"Found hWnd %d\n", hCurWnd);
    }
  } while (hCurWnd != nullptr);
}

inline void ActivateWindow(HWND mainWindowHandle) {
  // check if already has focus
  if (mainWindowHandle == GetForegroundWindow())
    return;

  // check if window is minimized
  if (IsIconic(mainWindowHandle)) {
    ShowWindow(mainWindowHandle, SW_RESTORE);
  }

  // Simulate a key press
  keybd_event(VK_MENU, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);

  // Simulate a key release
  keybd_event(VK_MENU, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);

  SetForegroundWindow(mainWindowHandle);
}

inline void ActivateAllWindowsByProcessHandle(HANDLE hProcess) {
  std::vector<HWND> vet;
  auto pid = GetProcessId(hProcess);
  AllowSetForegroundWindow(pid);
  Sleep(1000);
  GetAllWindowsFromProcessID(pid, vet);
  for (const auto &item : vet) {
    ActivateWindow((HWND)item);
  }
}

inline HINSTANCE ExecuteW(HWND hwnd, LPCWSTR szBuffer, LPCWSTR lpParameters,
                          INT nShowCmd, LPCWSTR lpDirectory) {
  int iReturn = -1;
  SHELLEXECUTEINFOW shExInfo = {0};
  shExInfo.cbSize = sizeof(shExInfo);
  shExInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
  shExInfo.hwnd = 0;
  shExInfo.lpVerb = IsWindowsXP() ? L"open" : L"runas"; // Operation to perform
  shExInfo.lpFile = szBuffer;                           // Application to start
  shExInfo.lpParameters = lpParameters;                 // Additional parameters
  shExInfo.lpDirectory = lpDirectory;
  shExInfo.nShow = nShowCmd;
  shExInfo.hInstApp = 0;

  if (ShellExecuteExW(&shExInfo)) {
    ActivateAllWindowsByProcessHandle(shExInfo.hProcess);
    return shExInfo.hInstApp;
  }
  return (HINSTANCE)-1;
}

std::int64_t execute_command(const std::string &cmd,
                             const std::string &parameters,
                             const std::string &working_directory) {
  const auto &cmdWide = utf8_to_utf16(cmd);
  const auto &parametersWide = utf8_to_utf16(parameters);
  const auto &workingDirectoryWide = utf8_to_utf16(working_directory);
  return (std::int64_t)ExecuteW(
      NULL, (LPCWSTR)cmdWide.data(), (LPCWSTR)parametersWide.data(),
      SW_SHOWMAXIMIZED, (LPCWSTR)workingDirectoryWide.data());
}

std::int64_t execute_command(const std::string &cmd,
                             const std::vector<std::string> &parameters,
                             const std::string &working_directory) {
  const auto &joinedString = sprintor::utils::string::join(
      parameters.begin(), parameters.end(), std::string(" "));
  return execute_command(cmd, joinedString, working_directory);
}

} // namespace process
} // namespace interop
} // namespace sprintor

#endif