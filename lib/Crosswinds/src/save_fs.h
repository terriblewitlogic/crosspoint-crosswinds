// Where the save files live. The standalone firmware and the desktop host keep
// them in LittleFS. Inside CrossPoint they go to the SD card through
// CrossPoint's storage layer (crosspoint_fs_* in the fork's
// CrosswindsActivity.cpp), which serialises card access with the reader; the
// names save.cpp uses ("/sh_company.sav" and so on) land in one folder there.
#pragma once
#include "profile.h"

#if defined(SH_CROSSPOINT)
#include <stddef.h>
#include <stdint.h>

struct CrosspointFile;  // owned by the CrossPoint side
bool crosspoint_fs_begin();
bool crosspoint_fs_exists(const char* path);
bool crosspoint_fs_remove(const char* path);
bool crosspoint_fs_rename(const char* from, const char* to);
CrosspointFile* crosspoint_fs_open(const char* path, const char* mode);  // "r", "w" or "r+"
size_t crosspoint_fs_read(CrosspointFile* file, void* buf, size_t len);
size_t crosspoint_fs_write(CrosspointFile* file, const void* buf, size_t len);
bool crosspoint_fs_seek(CrosspointFile* file, uint32_t pos);
void crosspoint_fs_flush(CrosspointFile* file);
void crosspoint_fs_close(CrosspointFile* file);

// The slice of Arduino's fs::File that save.cpp uses.
class SaveFile {
  CrosspointFile* file = nullptr;
 public:
  SaveFile() = default;
  explicit SaveFile(CrosspointFile* f) : file(f) {}
  SaveFile(SaveFile&& other) : file(other.file) { other.file = nullptr; }
  SaveFile& operator=(SaveFile&& other) {
    if (this != &other) { close(); file = other.file; other.file = nullptr; }
    return *this;
  }
  SaveFile(const SaveFile&) = delete;
  SaveFile& operator=(const SaveFile&) = delete;
  ~SaveFile() { close(); }
  explicit operator bool() const { return file != nullptr; }
  size_t read(void* buf, size_t len) { return file ? crosspoint_fs_read(file, buf, len) : 0; }
  size_t write(const void* buf, size_t len) { return file ? crosspoint_fs_write(file, buf, len) : 0; }
  bool seek(uint32_t pos) { return file && crosspoint_fs_seek(file, pos); }
  void flush() { if (file) crosspoint_fs_flush(file); }
  void close() { if (file) { crosspoint_fs_close(file); file = nullptr; } }
};

struct SaveFs {
  bool begin(bool) { return crosspoint_fs_begin(); }
  bool exists(const char* path) { return crosspoint_fs_exists(path); }
  bool remove(const char* path) { return crosspoint_fs_remove(path); }
  bool rename(const char* from, const char* to) { return crosspoint_fs_rename(from, to); }
  SaveFile open(const char* path, const char* mode) { return SaveFile(crosspoint_fs_open(path, mode)); }
};
inline SaveFs saveFs;
#define SAVE_FS saveFs

#else
#include <LittleFS.h>
#define SAVE_FS LittleFS
using SaveFile = File;
#endif
