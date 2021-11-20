#if __APPLE__
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <ios>
#include <stdexcept>
#include <string>

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <mach-o/dyld.h>
#include <mach-o/loader.h>
#include <mach-o/nlist.h>

#include <crossguid/guid.hpp>

#include <sprintor/interop/file_system.h>
#include <sprintor/interop/process.h>

#define EXECUTABLE_BASE_ADDR 0x100000000
#define DYLD_BASE 0x00007fff5fc00000

int IS_SIERRA = -1;

// returns 1 if running on Sierra, 0 otherwise
// this works because /bin/rcp was removed in Sierra
int is_sierra(void) {
  if (IS_SIERRA == -1) {
    struct stat statbuf;
    IS_SIERRA = (stat("/bin/rcp", &statbuf) != 0);
  }
  return IS_SIERRA;
}

// find a Mach-O header by searching from address.
int find_macho(unsigned long addr, unsigned long *base, unsigned int increment,
               unsigned int dereference) {
  unsigned long ptr;

  *base = 0;

  while (1) {
    ptr = addr;
    if (dereference)
      ptr = *(unsigned long *)ptr;
    chmod((char *)ptr, 0777);
    // No such file or directory
    if (errno == ENOENT && ((int *)ptr)[0] == MH_MAGIC_64) {
      *base = ptr;
      return 0;
    }

    addr += increment;
  }

  return 1;
}

// find the entry point command by searching through base's load commands
int find_epc(unsigned long base, struct entry_point_command **entry) {
  struct mach_header_64 *mh;
  struct load_command *lc;

  unsigned long text = 0;

  *entry = NULL;

  mh = (struct mach_header_64 *)base;
  lc = (struct load_command *)(base + sizeof(struct mach_header_64));
  for (int i = 0; i < mh->ncmds; i++) {
    if (lc->cmd == LC_MAIN) { // 0x80000028
      *entry = (struct entry_point_command *)lc;
      return 0;
    }

    lc = (struct load_command *)((unsigned long)lc + lc->cmdsize);
  }

  return 1;
}

// Parse the symbols in the Mach-O image at base and return the address of the
// one matched by the offset / int pair (offset, match)
unsigned long resolve_symbol(unsigned long base, unsigned int offset,
                             unsigned int match) {
  struct load_command *lc;
  struct segment_command_64 *sc, *linkedit, *text;
  struct symtab_command *symtab;
  struct nlist_64 *nl;

  char *strtab;

  symtab = 0;
  linkedit = 0;
  text = 0;

  lc = (struct load_command *)(base + sizeof(struct mach_header_64));
  for (int i = 0; i < ((struct mach_header_64 *)base)->ncmds; i++) {
    if (lc->cmd == 0x2 /*LC_SYMTAB*/) {
      symtab = (struct symtab_command *)lc;
    } else if (lc->cmd == 0x19 /*LC_SEGMENT_64*/) {
      sc = (struct segment_command_64 *)lc;
      switch (*((unsigned int *)&((struct segment_command_64 *)lc)
                    ->segname[2])) { // skip __
      case 0x4b4e494c:               // LINK
        linkedit = sc;
        break;
      case 0x54584554: // TEXT
        text = sc;
        break;
      }
    }
    lc = (struct load_command *)((unsigned long)lc + lc->cmdsize);
  }

  if (!linkedit || !symtab || !text)
    return -1;

  unsigned long file_slide =
      linkedit->vmaddr - text->vmaddr - linkedit->fileoff;
  strtab = (char *)(base + file_slide + symtab->stroff);

  nl = (struct nlist_64 *)(base + file_slide + symtab->symoff);
  for (int i = 0; i < symtab->nsyms; i++) {
    char *name = strtab + nl[i].n_un.n_strx;
    if (*(unsigned int *)&name[offset] == match) {
      if (is_sierra()) {
        return base + nl[i].n_value;
      }
      return base - DYLD_BASE + nl[i].n_value;
    }
  }

  return -1;
}

// Load the binary specified by filename using dyld
template <typename fp_t>
fp_t *get_proc_address(NSModule nm, const char *fname) {
  NSSymbol symbol = NSLookupSymbolInModule(nm, fname);
  if (!symbol) {
    return nullptr;
  }

  auto fp = (fp_t *)NSAddressOfSymbol(symbol);
  if (!fp) {
    return nullptr;
  }

  return fp;
}

template <typename fp_t> fp_t *find_entrypoint_fp(unsigned long nm) {
  unsigned long execute_base;
  struct entry_point_command *epc;

  if (find_macho(nm, &execute_base, sizeof(int), 1)) {
    return nullptr;
  }

  if (find_epc(execute_base, &epc)) {
    return nullptr;
  }

  return (fp_t *)(execute_base + epc->entryoff);
}

int load_and_exec(const char *module_name, void *binbuf, std::size_t size,
                  int argc, char *argv[], char *env[], char *apple[]) {
  using main_fp_t = int(int, char **, char **, char **);

  // change the filetype to a bundle
  int type = ((int *)binbuf)[3];
  if (type != MH_BUNDLE) {
    // change to mh_bundle type
    ((int *)binbuf)[3] = 0x8;
  }

  // create file image
  NSObjectFileImage fi;
  if (NSCreateObjectFileImageFromMemory(binbuf, size, &fi) !=
      NSObjectFileImageReturnCode::NSObjectFileImageSuccess) {
    throw std::runtime_error("Could not create image.");
  }

  // link image
  NSModule nm =
      NSLinkModule(fi, module_name,
                   NSLINKMODULE_OPTION_PRIVATE | NSLINKMODULE_OPTION_BINDNOW);
  if (!nm) {
    throw std::runtime_error("Could not link image.");
  }

  // find entry point and call it
  if (type == MH_EXECUTE) {
    auto main = get_proc_address<main_fp_t>(nm, "_main");
    if (!main) {
      main = find_entrypoint_fp<main_fp_t>((unsigned long)nm);
    }

    return main(argc, argv, env, apple);
  }

  return 1;
}

namespace sprintor {
namespace interop {
namespace process {

void release_hmodule(std::uint64_t hmodule) { return; }

std::int64_t memexec(const std::string &file_name, void *bin_buffer,
                     std::size_t exe_size, int argc, char **argv) {
  return load_and_exec(file_name.c_str(), bin_buffer, exe_size, argc, argv,
                       NULL, NULL);
}

std::int64_t memexec(const std::string &file_name, void *exe,
                     std::size_t exe_size,
                     const std::vector<std::string> &argv) {
  char **args_array;
  int argc;
  if (convert_to_argv(argv, &args_array, &argc)) {
    auto hmodule = memexec(file_name, exe, exe_size, argc, args_array);
    free_buffer(&args_array, argc);
    return (std::uint64_t)hmodule;
  }
  return -1;
}

} // namespace process
} // namespace interop
} // namespace sprintor
#endif