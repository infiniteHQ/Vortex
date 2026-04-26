//
//  vortex.h
//  Root header files, including all the vortex back-end features
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

// (Integer encoded as XYYZZ for use in #if preprocessor conditionals, e.g. '#if
// VORTEX_VERSION_NUM >= 12345')
#define VORTEX_VERSION     "1.0"
#define VORTEX_VERSION_NUM 1000

#pragma once

#include "vortex_configs.h"

// Includes
#include <stddef.h>
#include <stdio.h>
#include <sys/stat.h>

#include <algorithm>
#include <chrono>
#include <condition_variable>
#include <cstring>
#include <ctime>
#include <deque>
#include <filesystem>
#include <fstream>
#include <future>
#include <iostream>
#include <mutex>
#include <random>
#include <regex>
#include <stdexcept>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#if !defined(_WIN32) || !defined(_WIN64)
#include <dirent.h>
#include <dlfcn.h>
#include <unistd.h>
#endif

// Third party includes
#include "../../lib/cherry/cherry.hpp"
#include "../../lib/json/single_include/nlohmann/json.hpp"
#include "../../lib/spdlog/include/spdlog/sinks/basic_file_sink.h"
#include "../../lib/spdlog/include/spdlog/sinks/daily_file_sink.h"
#include "../../lib/spdlog/include/spdlog/sinks/rotating_file_sink.h"
#include "../../lib/spdlog/include/spdlog/sinks/stdout_color_sinks.h"
#include "../../lib/spdlog/include/spdlog/spdlog.h"

namespace fs = std::filesystem;

#ifndef VORTEX_API
#define VORTEX_API
#endif
#ifndef VORTEX_EXT_API
#define VORTEX_EXT_API VORTEX_API
#endif

// Helper Macros
#ifndef VX_ASSERT
#include <assert.h>
#define VX_ASSERT(_EXPR) assert(_EXPR)
#endif
#define VX_ARRAYSIZE(_ARR) \
  ((int)(sizeof(_ARR) / sizeof(*(_ARR))))  // Size of a static C-style array.
                                           // Don't use on pointers!
#define VX_UNUSED(_VAR) \
  ((void)(_VAR))  // Used to silence "unused variable warnings". Often useful as
                  // asserts may be stripped out from final builds.
#define VX_OFFSETOF(_TYPE, _MEMBER) \
  offsetof(_TYPE, _MEMBER)  // Offset of _MEMBER within _TYPE. Standardized as
                            // offsetof() in C++11
#define VX_CHECKVERSION() vxe::DebugCheckVersionAndDataLayout(VORTEX_VERSION)  // Version

// Forward declarations
template<typename T>
struct hVector;

template<typename K, typename V>
struct hMap;

struct hString;
struct hArgs;
struct EnvProject;

struct ArgumentValues;
struct ReturnValues;
struct Theme;

struct CommandOutput;
struct ItemHandlerInterface;

// Internals (from vortex_internals.h)
struct VxContext;

// Callback and functions types
typedef void *(*VortexMakerMemAllocFunc)(size_t sz, void *user_data);  // Function signature for
                                                                       // vxe::SetAllocatorFunctions()
typedef void (*VortexMakerMemFreeFunc)(void *ptr, void *user_data);    // Function signature for
                                                                       // vxe::SetAllocatorFunctions()

namespace vxe {
  // TODO -> Only that on vortex.cpp (must be smart pointers)
  VORTEX_API VxContext *create_context();
  VORTEX_API void destroy_context(VxContext *context = NULL);
  VORTEX_API VxContext *get_current_context();
  VORTEX_API void set_current_context(VxContext *context);

  VORTEX_API void initialize();

  VORTEX_API bool IsThemeNeedsRebuild();
  VORTEX_API void ThemeRebuilded();
  VORTEX_API void RebuildTheme();
  VORTEX_API void RefreshProjectThemes();
  VORTEX_API void UpdateProjectTheme(const std::shared_ptr<Theme> &theme);
  VORTEX_API void UpdateProjectThemesComfig();
  VORTEX_API void VerifyAndPouplateThemes();

  VORTEX_API void CreateNewTheme(const std::shared_ptr<Theme> &base_theme, const std::string &title);

  VORTEX_API std::shared_ptr<Theme> GetTheme(const std::string &label);
  VORTEX_API std::shared_ptr<Theme> GetSelectedTheme();

  VORTEX_API std::vector<std::shared_ptr<ItemHandlerInterface>> GetAllItemHandlersFor(const std::string &type);

  VORTEX_API void InstallModuleToSystem(const std::string &path);

  VORTEX_API void InstallModule(const std::string &module_name, const std::string &version, bool &restart_modules);
  VORTEX_API void AddModuleToProject(const std::string &module_name);

  VORTEX_API void InstallPluginToSystem(const std::string &path);

  VORTEX_API void InstallPlugin(const std::string &plugin_name, const std::string &version, bool &restart_plugins);
  VORTEX_API void AddPluginToProject(const std::string &plugin_name);

  VORTEX_API void RemoveSystemProjectEntry(const std::string &project_name);

  VORTEX_API void InstallContentOnSystem(const std::string &directory);

  VORTEX_API void ExecuteStartScript();

  VORTEX_API std::string getCurrentTimeStamp();
  VORTEX_API void OpenURL(const std::string &url);

  bool DebugCheckVersionAndDataLayout(const char *version);

  VORTEX_API std::string CreateFile(const std::string &path);
  VORTEX_API std::string CreateFolder(const std::string &path);
  VORTEX_API void MoveAllContent();
  VORTEX_API void CopyAllContent();
  VORTEX_API void ExecuteCommand();

  // utils
  VORTEX_API std::string ConvertPathToWindowsStyle(const std::string &path);

  VORTEX_API void SubmitRename(const std::string &oldPathStr, const std::string &newName);

  // credits
  VORTEX_API void AddCredits(const std::string &topic, const std::string &credit_file);
  VORTEX_API void SetCreditsFile(const std::string &topic, const std::string &credit_file);

  // documentation
  VORTEX_API void AddDocumentation(
      const std::string &topic,
      const std::string &section,
      const std::string &title,
      const std::string &md_file_path);
  VORTEX_API std::vector<std::string> GetSections(const std::string &topic);
  VORTEX_API std::vector<std::string> GetChapters(const std::string &topic, const std::string &section);
  VORTEX_API std::string GetChapterFilePath(const std::string &topic, const std::string &section, const std::string &title);
  VORTEX_API std::vector<std::string> GetTitlesFromTopic(const std::string &topic);
  VORTEX_API std::vector<std::string> GetNamesFromTopicAndTitle(const std::string &topic, const std::string &title);
  VORTEX_API void AddVortexDocumentation();

  VORTEX_API void PushEditMenuItem(
      const std::string &title,
      const std::function<void()> &action,
      const std::string &logo = "",
      const std::string &section = "");
  VORTEX_API void PopEditMenuItem(const int &count = 1);
  VORTEX_API void ClearEditMenuItem();

  VORTEX_API void PushCustomMenu(const std::string &title, const std::function<void()> &render);
  VORTEX_API void PopCustomMenu(const int &count = 1);
  VORTEX_API void ClearCustomMenus();

  VORTEX_API std::string ExtractPackageWithTar(const std::string &path, const std::string &tarballName);
  VORTEX_API std::string replacePlaceholders(
      const std::string &command,
      const std::unordered_map<std::string, std::string> &replacements);

  // Memory Allocators
  // - Those functions are not reliant on the current context.
  // - DLL users: heaps and globals are not shared across DLL boundaries! You
  // will need to call set_current_context() + SetAllocatorFunctions()
  //   for each static/DLL boundary you are calling from. Read "Context and
  //   Memory Allocators" section of VortexMaker.cpp for more details.
  VORTEX_API void
  SetAllocatorFunctions(VortexMakerMemAllocFunc alloc_func, VortexMakerMemFreeFunc free_func, void *user_data = NULL);
  VORTEX_API void
  GetAllocatorFunctions(VortexMakerMemAllocFunc *p_alloc_func, VortexMakerMemFreeFunc *p_free_func, void **p_user_data);
  VORTEX_API void *MemAlloc(size_t size);
  VORTEX_API void MemFree(void *ptr);

}  // namespace vxe
//_____________________________________________________________________________

struct VxNewWrapper { };
inline void *operator new(size_t, VxNewWrapper, void *ptr) {
  return ptr;
}
inline void operator delete(void *, VxNewWrapper, void *) {
}  // This is only required so we can use the symmetrical new()
#define VX_ALLOC(_SIZE)        vxe::MemAlloc(_SIZE)
#define VX_FREE(_PTR)          vxe::MemFree(_PTR)
#define VX_PLACEMENT_NEW(_PTR) new (VxNewWrapper(), _PTR)
#define VX_NEW(_TYPE)          new (VxNewWrapper(), vxe::MemAlloc(sizeof(_TYPE))) _TYPE
template<typename T>
void VX_DELETE(T *p) {
  if (p) {
    p->~T();
    vxe::MemFree(p);
  }
}

//=============================================================================
// hVector<>
// Lightweight hVector<>-like class to avoid dragging dependencies (also, some
// implementations of STL with debug enabled are absurdly slow, we bypass it so
// our code runs fast in debug).
//=============================================================================
// - You generally do NOT need to care or use this ever. But we need to make it
// available in imgui.h because some of our public structures are relying on it.
// - We use std-like naming convention here, which is a little unusual for this
// codebase.
// - Important: clear() frees memory, resize(0) keep the allocated buffer. We
// use resize(0) a lot to intentionally recycle allocated buffers across frames
// and amortize our costs.
// - Important: our implementation does NOT call C++ constructors/destructors,
// we treat everything as raw data! This is intentional but be extra mindful of
// that,
//   Do NOT use this class as a std::vector replacement in your own code! Many
//   of the structures used by dear imgui can be safely initialized by a
//   zero-memset.
//=============================================================================

template<typename T>
struct hVector {
  int Size;
  int Capacity;
  T *Data;

  // Provide standard typedefs but we don't use them ourselves.
  typedef T value_type;
  typedef value_type *iterator;
  typedef const value_type *const_iterator;

  // Constructors, destructor
  inline hVector(std::initializer_list<T> initList) : Size(0), Capacity(0), Data(nullptr) {
    reserve(initList.size());
    for (const T &value : initList) {
      new (&Data[Size++]) T(value);
    }
  }
  inline hVector() {
    Size = Capacity = 0;
    Data = NULL;
  }
  inline hVector(const hVector<T> &src) {
    Size = Capacity = 0;
    Data = NULL;
    operator=(src);
  }
  inline hVector<T> &operator=(const hVector<T> &src) {
    clear();
    resize(src.Size);
    if (src.Data)
      memcpy(Data, src.Data, (size_t)Size * sizeof(T));
    return *this;
  }
  inline ~hVector() {
    if (Data)
      VX_FREE(Data);
  }  // Important: does not destruct anything

  inline void clear() {
    if (Data) {
      Size = Capacity = 0;
      VX_FREE(Data);
      Data = NULL;
    }
  }  // Important: does not destruct anything
  inline void clear_delete() {
    for (int n = 0; n < Size; n++)
      IM_DELETE(Data[n]);
    clear();
  }  // Important: never called automatically! always explicit.
  inline void clear_destruct() {
    for (int n = 0; n < Size; n++) {
      Data[n].~T();
    }
    clear();
  }

  inline bool empty() const {
    return Size == 0;
  }
  inline int size() const {
    return Size;
  }
  inline int size_in_bytes() const {
    return Size * (int)sizeof(T);
  }
  inline int max_size() const {
    return 0x7FFFFFFF / (int)sizeof(T);
  }
  inline int capacity() const {
    return Capacity;
  }
  inline T &operator[](int i) {
    VX_ASSERT(i >= 0 && i < Size);
    return Data[i];
  }
  inline const T &operator[](int i) const {
    VX_ASSERT(i >= 0 && i < Size);
    return Data[i];
  }

  inline T *begin() {
    return Data;
  }
  inline const T *begin() const {
    return Data;
  }
  inline T *end() {
    return Data + Size;
  }
  inline const T *end() const {
    return Data + Size;
  }
  inline T &front() {
    VX_ASSERT(Size > 0);
    return Data[0];
  }
  inline const T &front() const {
    VX_ASSERT(Size > 0);
    return Data[0];
  }
  inline T &back() {
    VX_ASSERT(Size > 0);
    return Data[Size - 1];
  }
  inline const T &back() const {
    VX_ASSERT(Size > 0);
    return Data[Size - 1];
  }
  inline void swap(hVector<T> &rhs) {
    int rhs_size = rhs.Size;
    rhs.Size = Size;
    Size = rhs_size;
    int rhs_cap = rhs.Capacity;
    rhs.Capacity = Capacity;
    Capacity = rhs_cap;
    T *rhs_data = rhs.Data;
    rhs.Data = Data;
    Data = rhs_data;
  }

  inline int _grow_capacity(int sz) const {
    int new_capacity = Capacity ? (Capacity + Capacity / 2) : 8;
    return new_capacity > sz ? new_capacity : sz;
  }
  inline void resize(int new_size) {
    if (new_size > Capacity)
      reserve(_grow_capacity(new_size));
    Size = new_size;
  }

  inline void resize(int new_size, const T &v) {
    if (new_size > Capacity)
      reserve(_grow_capacity(new_size));
    if (new_size > Size) {
      for (int n = Size; n < new_size; n++) {
        new (&Data[n]) T(v);
      }
    }
    Size = new_size;
  }
  inline void shrink(int new_size) {
    VX_ASSERT(new_size <= Size);
    Size = new_size;
  }  // Resize a vector to a smaller size, guaranteed not to cause a reallocation
  inline void reserve(int new_capacity) {
    if (new_capacity <= Capacity)
      return;
    T *new_data = (T *)VX_ALLOC((size_t)new_capacity * sizeof(T));
    if (Data) {
      memcpy(new_data, Data, (size_t)Size * sizeof(T));
      VX_FREE(Data);
    }
    Data = new_data;
    Capacity = new_capacity;
  }
  inline void reserve_discard(int new_capacity) {
    if (new_capacity <= Capacity)
      return;
    if (Data)
      VX_FREE(Data);
    Data = (T *)VX_ALLOC((size_t)new_capacity * sizeof(T));
    Capacity = new_capacity;
  }

  // NB: It is illegal to call push_back/push_front/insert with a reference
  // pointing inside the hVector data itself! e.g. v.push_back(v[10]) is
  // forbidden.
  inline void push_back(const T &v) {
    if (Size == Capacity)
      reserve(_grow_capacity(Size + 1));
    new (&Data[Size]) T(v);
    Size++;
  }
  inline void pop_back() {
    VX_ASSERT(Size > 0);
    Size--;
  }
  inline void push_front(const T &v) {
    if (Size == 0)
      push_back(v);
    else
      insert(Data, v);
  }

  inline T *erase(const T *it) {
    VX_ASSERT(it >= Data && it < Data + Size);
    const ptrdiff_t off = it - Data;
    Data[off].~T();
    memmove(Data + off, Data + off + 1, ((size_t)Size - (size_t)off - 1) * sizeof(T));
    Size--;
    return Data + off;
  }
  inline T *erase(const T *it, const T *it_last) {
    VX_ASSERT(it >= Data && it < Data + Size && it_last >= it && it_last <= Data + Size);
    const ptrdiff_t count = it_last - it;
    const ptrdiff_t off = it - Data;
    memmove(Data + off, Data + off + count, ((size_t)Size - (size_t)off - (size_t)count) * sizeof(T));
    Size -= (int)count;
    return Data + off;
  }
  inline T *erase_unsorted(const T *it) {
    VX_ASSERT(it >= Data && it < Data + Size);
    const ptrdiff_t off = it - Data;
    if (it < Data + Size - 1)
      memcpy(Data + off, Data + Size - 1, sizeof(T));
    Size--;
    return Data + off;
  }
  inline T *insert(const T *it, const T &v) {
    VX_ASSERT(it >= Data && it <= Data + Size);
    const ptrdiff_t off = it - Data;
    if (Size == Capacity)
      reserve(_grow_capacity(Size + 1));
    if (off < (int)Size)
      memmove(Data + off + 1, Data + off, ((size_t)Size - (size_t)off) * sizeof(T));
    memcpy(&Data[off], &v, sizeof(v));
    Size++;
    return Data + off;
  }
  inline bool contains(const T &v) const {
    const T *data = Data;
    const T *data_end = Data + Size;
    while (data < data_end)
      if (*data++ == v)
        return true;
    return false;
  }
  inline T *find(const T &v) {
    T *data = Data;
    const T *data_end = Data + Size;
    while (data < data_end)
      if (*data == v)
        break;
      else
        ++data;
    return data;
  }
  inline const T *find(const T &v) const {
    const T *data = Data;
    const T *data_end = Data + Size;
    while (data < data_end)
      if (*data == v)
        break;
      else
        ++data;
    return data;
  }
  inline int find_index(const T &v) const {
    const T *data_end = Data + Size;
    const T *it = find(v);
    if (it == data_end)
      return -1;
    const ptrdiff_t off = it - Data;
    return (int)off;
  }
  inline bool find_erase(const T &v) {
    const T *it = find(v);
    if (it < Data + Size) {
      erase(it);
      return true;
    }
    return false;
  }
  inline bool find_erase_unsorted(const T &v) {
    const T *it = find(v);
    if (it < Data + Size) {
      erase_unsorted(it);
      return true;
    }
    return false;
  }
  inline int index_from_ptr(const T *it) const {
    VX_ASSERT(it >= Data && it < Data + Size);
    const ptrdiff_t off = it - Data;
    return (int)off;
  }
};
//=============================================================================

//=============================================================================
// hMap<>
//=============================================================================
template<typename K, typename V>
class hMap {
 private:
  struct KeyValuePair {
    K key;
    V value;
  };
  hVector<KeyValuePair> data;

 public:
  inline void insert(const K &key, const V &value) {
    for (int i = 0; i < data.size(); ++i) {
      if (data[i].key == key) {
        data[i].value = value;
        return;
      }
    }
    KeyValuePair pair;
    pair.key = key;
    pair.value = value;
    data.push_back(pair);
  }
  inline bool find(const K &key, V &value) const {
    for (int i = 0; i < data.size(); ++i) {
      if (data[i].key == key) {
        value = data[i].value;
        return true;
      }
    }
    return false;
  }
  inline bool remove(const K &key) {
    for (int i = 0; i < data.size(); ++i) {
      if (data[i].key == key) {
        data.erase(data.begin() + i);
        return true;
      }
    }
    return false;
  }
  inline int size() const {
    return data.size();
  }
  inline const V &find(const K &key) const {
    for (int i = 0; i < data.size(); ++i) {
      if (data[i].key == key) {
        return data[i].value;
      }
    }
    throw std::out_of_range("Key not found in hMap");
  }
  inline const V &at(const K &key) const {
    for (int i = 0; i < data.size(); ++i) {
      if (data[i].key == key) {
        return data[i].value;
      }
    }
    throw std::out_of_range("Key not found in hMap");
  }

  bool operator!=(const K &key) const {
    for (int i = 0; i < data.size(); ++i) {
      if (data[i].key == key) {
        return false;
      }
    }
    return true;
  }
  V &operator[](const K &key) {
    for (int i = 0; i < data.size(); ++i) {
      if (data[i].key == key) {
        return data[i].value;
      }
    }
    KeyValuePair pair;
    pair.key = key;
    data.push_back(pair);
    return data.back().value;
  }

  typename hVector<KeyValuePair>::iterator begin() {
    return data.begin();
  }
  typename hVector<KeyValuePair>::iterator end() {
    return data.end();
  }
  typename hVector<KeyValuePair>::const_iterator begin() const {
    return data.begin();
  }
  typename hVector<KeyValuePair>::const_iterator end() const {
    return data.end();
  }
};
//=============================================================================

//=============================================================================
// hString<>
//=============================================================================
struct hString {
  hVector<char> Buf;
  VORTEX_API static char EmptyString[1];

  hString() : Data(nullptr), Size(0), Capacity(0) {
  }
  hString(const char *str) {
    this->append(str);
  }
  hString(int intValue) {
    char buffer[20];
    snprintf(buffer, sizeof(buffer), "%d", intValue);
    this->append(buffer);
  }

  size_t find(const char *substr, size_t start = 0) const {
    if (Buf.Data == nullptr || substr == nullptr || start >= Buf.Size) {
      return npos;
    }
    const char *result = std::strstr(Buf.Data + start, substr);
    if (result == nullptr) {
      return npos;
    }
    return result - Buf.Data;
  }
  size_t rfind(const char *substr, size_t start = npos) const {
    if (Buf.Data == nullptr || substr == nullptr) {
      return npos;
    }
    size_t searchStart = (start == npos) ? Size - 1 : (std::min)(start, Size - 1);
    for (size_t i = searchStart; i < Size; --i) {
      const char *result = std::strstr(Buf.Data + i, substr);
      if (result != nullptr) {
        return i;
      }
    }
    return npos;
  }
  hString substr(size_t pos, size_t len = npos) const {
    if (Buf.Data == nullptr || pos >= Buf.Size) {
      return hString();
    }
    if (len == npos || pos + len > Buf.Size) {
      len = Buf.Size - pos;
    }
    hString result;
    result.reserve(static_cast<int>(len));
    result.append(Buf.Data + pos, Buf.Data + pos + len);
    return result;
  }
  size_t find_first_of(const char *charsToFind, size_t start = 0) const {
    if (Data == nullptr || charsToFind == nullptr || start >= Size) {
      return npos;
    }
    for (size_t i = start; i < Size; ++i) {
      char c = Data[i];
      if (strchr(charsToFind, c) != nullptr) {
        return i;
      }
    }
    return npos;
  }
  int length() const {
    return static_cast<int>(Size);
  }
  void push_back(char c) {
    Buf.push_back(c);
  }
  const char *begin() const {
    return Buf.Data ? &Buf.front() : EmptyString;
  }
  const char *end() const {
    return Buf.Data ? &Buf.back() : EmptyString;
  }
  int size() const {
    return Buf.Size ? Buf.Size - 1 : 0;
  }
  bool empty() const {
    return Buf.Size <= 1;
  }
  void reserve(int capacity) {
    if (capacity <= Capacity) {
      return;
    }
    char *newData = new char[capacity];
    if (Data) {
      memcpy(newData, Data, Size);
      delete[] Data;
    }
    Data = newData;
    Capacity = capacity;
  }
  void clear() {
    if (Data) {
      delete[] Data;
      Data = nullptr;
      Size = 0;
      Capacity = 0;
    }
  }
  const char *c_str() const {
    return Buf.Data ? Buf.Data : EmptyString;
  }

  VORTEX_API void append(const char *str, const char *str_end = NULL);

  inline char operator[](int i) const {
    VX_ASSERT(Buf.Data != NULL);
    return Buf.Data[i];
  }
  bool operator==(const char *str) const {
    if (Buf.Data == nullptr && str == nullptr) {
      return true;
    }
    if (Buf.Data == nullptr || str == nullptr) {
      return false;
    }
    return strcmp(Buf.Data, str) == 0;
  }
  bool operator==(const hString &other) const {
    return operator==(other.c_str());
  }
  hString &operator=(const char *str) {
    this->clear();
    this->append(str);
    return *this;
  }
  hString &operator+=(int value) {
    char strValue[12];
    snprintf(strValue, sizeof(strValue), "%d", value);
    this->append(strValue);
    return *this;
  }
  hString &operator+=(const char *str) {
    this->append(str);
    return *this;
  }
  hString &operator+=(const hString &other) {
    this->append(other.c_str());
    return *this;
  }
  bool operator!=(const char *rhs) const {
    return strcmp(Data, rhs) != 0;
  }
  friend hString operator+(const char *lhs, const hString &rhs) {
    hString result(lhs);
    result.append(rhs.c_str());
    return result;
  }
  friend hString operator+(const hString &lhs, const char *rhs) {
    hString result(lhs);
    result.append(rhs);
    return result;
  }
  friend hString operator+(const hString &lhs, const hString &rhs) {
    hString result(lhs);
    result.append(rhs.c_str());
    return result;
  }

  static const size_t npos = static_cast<size_t>(-1);
  char *Data;
  size_t Size;
  size_t Capacity;
};
//=============================================================================

//=============================================================================
//  hArgs
//=============================================================================
class hArgs {
 private:
  class ArgumentBase {
   public:
    virtual ~ArgumentBase() = default;
  };

  template<typename T>
  class ArgumentHolder : public ArgumentBase {
   public:
    ArgumentHolder(T value) : storedValue(value) {
    }
    T getValue() const {
      return storedValue;
    }

   private:
    T storedValue;
  };

 public:
  hArgs() { };

  template<typename T>
  void add(const hString &tag, T value) {
    arguments[tag] = new ArgumentHolder<T>(value);
    registered_arguments.push_back(tag);
  }
  template<typename T>
  T get(const hString &tag, const T &defaultT) const {
    if (registered_arguments.contains(tag)) {
      auto it = arguments.find(tag);
      return dynamic_cast<ArgumentHolder<T> *>(it)->getValue();
    }
    return defaultT;
  }

  void remove(const hString &tag) {
    if (arguments.find(tag)) {
      auto it = arguments.find(tag);
      delete it;
      arguments.remove(tag);
      registered_arguments.erase(
          std::remove(registered_arguments.begin(), registered_arguments.end(), tag), registered_arguments.end());
    }
  }

  ~hArgs() {
    for (const auto &pair : arguments) {
      delete pair.value;
    }
  }

  hVector<hString> registered_arguments;

  hMap<hString, ArgumentBase *> arguments;
};
//=============================================================================

namespace vxe {
  /**
   * @brief Data structure for modules & plugins functions, events and other
   * stuff, oriented around JSON architecture to easely communicate complex types
   * or large amount of data. Formatted in std::string to increase the simplicity
   * of data transferts
   */
  struct Values {
   public:
    Values() { };
    Values(const std::string &val) : value(val) { };
    std::string GetValue() {
      return value;
    };
    nlohmann::json GetJsonValue() {
      return nlohmann::json::parse(value);
    };

    template<typename T>
    T GetJsonValue(const std::string &val) {
      return nlohmann::json::parse(val).get<T>();
    };

    void SetValue(const std::string &val) {
      value = val;
    };
    void SetJsonValue(const nlohmann::json &val) {
      value = val.dump();
    };

   private:
    std::string value = "null";
  };
}  // namespace vxe

struct ArgumentValues : public vxe::Values {
  using vxe::Values::Values;
};

struct ReturnValues : public vxe::Values {
  using vxe::Values::Values;
};