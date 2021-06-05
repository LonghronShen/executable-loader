#if defined(_WIN32) && (!defined(_WIN64))

#include <exception>
#include <fstream>
#include <stdexcept>
#include <vector>

#include <Windows.h>

#include <Shlwapi.h>

#include <sprintor/interop/process.h>
#include <sprintor/utils/string.h>

using namespace sprintor::utils::string;

namespace sprintor {
namespace interop {
namespace process {

typedef BOOL(WINAPI *pointMAIN)(int, char *);

typedef BOOL(WINAPI *pointWINMAIN)(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                                   LPSTR lpCmdLine, int nShowCmd);

inline DWORD GetSectionProtection(DWORD sc) {
  DWORD dwResult = 0;
  if (sc & IMAGE_SCN_MEM_NOT_CACHED)
    dwResult |= PAGE_NOCACHE;

  if (sc & IMAGE_SCN_MEM_EXECUTE) {
    if (sc & IMAGE_SCN_MEM_READ) {
      if (sc & IMAGE_SCN_MEM_WRITE)
        dwResult |= PAGE_EXECUTE_READWRITE;
      else
        dwResult |= PAGE_EXECUTE_READ;
    } else {
      if (sc & IMAGE_SCN_MEM_WRITE)
        dwResult |= PAGE_EXECUTE_WRITECOPY;
      else
        dwResult |= PAGE_EXECUTE;
    }
  } else {
    if (sc & IMAGE_SCN_MEM_READ) {
      if (sc & IMAGE_SCN_MEM_WRITE)
        dwResult |= PAGE_READWRITE;
      else
        dwResult |= PAGE_READONLY;
    } else {
      if (sc & IMAGE_SCN_MEM_WRITE)
        dwResult |= PAGE_WRITECOPY;
      else
        dwResult |= PAGE_NOACCESS;
    }
  }

  return dwResult;
}

inline BOOL IsImportByOrdinal(DWORD ImportDescriptor) {
  return (ImportDescriptor & IMAGE_ORDINAL_FLAG32) != 0;
}

inline HMODULE MemLoadExe(PBYTE data, LPSTR lpCmdLine) {
  IMAGE_FILE_HEADER *pFileHeader = NULL;
  IMAGE_OPTIONAL_HEADER *pOptionalHeader = NULL;
  IMAGE_SECTION_HEADER *pSectionHeader = NULL;
  IMAGE_IMPORT_DESCRIPTOR *pImportDscrtr = NULL;
  USHORT e_lfanew = *((USHORT *)(data + 0x3c));
  PCHAR ImageBase = NULL;
  PCHAR SectionBase = NULL;

  DWORD dwSize, dwOldProt, ImageBaseDelta;
  int i;

  pFileHeader = (IMAGE_FILE_HEADER *)(data + e_lfanew + 4);
  pOptionalHeader = (IMAGE_OPTIONAL_HEADER *)(data + e_lfanew + 4 +
                                              sizeof(IMAGE_FILE_HEADER));

  if (pOptionalHeader->Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC)
    return NULL;

  //    Let's try to reserv memory
  ImageBase = (PCHAR)VirtualAlloc((PVOID)pOptionalHeader->ImageBase,
                                  pOptionalHeader->SizeOfImage, MEM_RESERVE,
                                  PAGE_NOACCESS);

  if (ImageBase == NULL) {
    ImageBase = (PCHAR)VirtualAlloc(NULL, pOptionalHeader->SizeOfImage,
                                    MEM_RESERVE, PAGE_NOACCESS);
    if (ImageBase == NULL)
      return NULL;
  }

  //    copy the header
  SectionBase = (PCHAR)VirtualAlloc(ImageBase, pOptionalHeader->SizeOfHeaders,
                                    MEM_COMMIT, PAGE_READWRITE);
  memcpy(SectionBase, data, pOptionalHeader->SizeOfHeaders);
  //    Do headers read-only (to be on the safe side)
  VirtualProtect(SectionBase, pOptionalHeader->SizeOfHeaders, PAGE_READONLY,
                 &dwOldProt);

  //    find sections ...
  pSectionHeader = (IMAGE_SECTION_HEADER *)(pOptionalHeader + 1);
  for (i = 0; i < pFileHeader->NumberOfSections; i++) {
    SectionBase = (PCHAR)VirtualAlloc(
        ImageBase + pSectionHeader[i].VirtualAddress,
        pSectionHeader[i].Misc.VirtualSize, MEM_COMMIT, PAGE_READWRITE);
    if (SectionBase == NULL) {
      VirtualFree(ImageBase, 0, MEM_RELEASE);
      return NULL;
    }
    //    ... and copy initialization data
    SectionBase = ImageBase + pSectionHeader[i].VirtualAddress;
    dwSize = std::min(pSectionHeader[i].SizeOfRawData,
                      pSectionHeader[i].Misc.VirtualSize);
    memcpy(SectionBase, data + pSectionHeader[i].PointerToRawData, dwSize);
  }

  pImportDscrtr =
      (IMAGE_IMPORT_DESCRIPTOR
           *)(ImageBase +
              pOptionalHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT]
                  .VirtualAddress);

  for (; pImportDscrtr->Name != 0; pImportDscrtr++) {
    PCHAR pLibName = (PCHAR)(ImageBase + pImportDscrtr->Name);
    PCHAR pImortName = NULL;
    HMODULE hLibModule = LoadLibraryA(pLibName);
    DWORD *pImport = NULL, *pAddress = NULL;
    DWORD ProcAddress;

    pAddress = (DWORD *)(ImageBase + pImportDscrtr->/*Original*/ FirstThunk);
    if (pImportDscrtr->TimeDateStamp == 0)
      pImport = (DWORD *)(ImageBase + pImportDscrtr->FirstThunk);
    else
      pImport = (DWORD *)(ImageBase + pImportDscrtr->OriginalFirstThunk);
    for (i = 0; pImport[i] != 0; i++) {
      if (IsImportByOrdinal(pImport[i]))
        ProcAddress =
            (DWORD)GetProcAddress(hLibModule, (PCHAR)(pImport[i] & 0xFFFF));
      else // import by name
      {
        pImortName = (PCHAR)(ImageBase + (pImport[i]) + 2);
        ProcAddress = (DWORD)GetProcAddress(hLibModule, pImortName);
      }
      pAddress[i] = ProcAddress;
    }
  }

  //    set section protection
  for (i = 0; i < pFileHeader->NumberOfSections; i++)
    VirtualProtect((PVOID)(ImageBase + pSectionHeader[i].VirtualAddress),
                   pSectionHeader[i].Misc.VirtualSize,
                   GetSectionProtection(pSectionHeader[i].Characteristics),
                   &dwOldProt);

  //    call MainFunc
  if (pOptionalHeader->AddressOfEntryPoint != 0) {
    // pointMAIN
    // pWinMain=(pointMAIN)(ImageBase+pOptionalHeader->AddressOfEntryPoint);
    pointWINMAIN pWinMain =
        (pointWINMAIN)(ImageBase + pOptionalHeader->AddressOfEntryPoint);
    if (!pWinMain((HINSTANCE)ImageBase, NULL, lpCmdLine, SW_SHOWNORMAL)) {
      VirtualFree(ImageBase, 0, MEM_RELEASE);
      return NULL;
    }
  }

  return (HMODULE)ImageBase;
}

void release_hmodule(std::uint64_t hmodule) {
  VirtualFree((LPVOID)hmodule, 0, MEM_RELEASE);
}

std::uint64_t memexec(const std::string &file_name, void *exe,
                      std::size_t exe_size,
                      const std::vector<std::string> &argv) {
  const auto &cmd = convert_to_cmd(argv);
  auto hmodule = MemLoadExe((PBYTE)exe, (LPSTR)cmd.c_str());
  return (std::uint64_t)hmodule;
}

std::uint64_t memexec(const std::string &file_name, void *exe,
                      std::size_t exe_size, const char **argv) {
  const auto &args = extract_from_argv(argv);
  return memexec(file_name, exe, exe_size, args);
}

} // namespace process
} // namespace interop
} // namespace sprintor

#endif