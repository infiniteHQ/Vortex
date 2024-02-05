// The VortexMaker Project, version 1.0
// [header files]

// See licence in LICSENCE.md
// (c) 2022-2024, Diego E. Moreno

//_____________________________________________________________________________
// Introduction
//_____________________________________________________________________________
//_____________________________________________________________________________




//_____________________________________________________________________________
// Project map
//_____________________________________________________________________________
//_____________________________________________________________________________




//_____________________________________________________________________________
// Ressources
//_____________________________________________________________________________
//_____________________________________________________________________________




//_____________________________________________________________________________
// Versions & Build identification
//_____________________________________________________________________________
// (Integer encoded as XYYZZ for use in #if preprocessor conditionals, e.g. '#if VORTEX_VERSION_NUM >= 12345')
#define VORTEX_VERSION       "1.0"
#define VORTEX_VERSION_NUM   10000

//_____________________________________________________________________________




//_____________________________________________________________________________
// Index of this file
//_____________________________________________________________________________
// [SECTION] Header mess
// [SECTION] Forward declarations of structures
// [SECTION] VortexMaker end-use API functions
// [SECTION] Flags & Enumerations
// [SECTION] Structures
//_____________________________________________________________________________


//_____________________________________________________________________________
// Configuration
//_____________________________________________________________________________
#pragma once
// Configuration file with compile-time options
// (edit Vxconfigs.h or '#define VORTEX_USER_CONFIG "myfilename.h" from your build system')
#ifdef VORTEX_USER_CONFIG
#include VORTEX_USER_CONFIG
#endif
#include "vortex_configs.h"

#ifndef VORTEX_DISABLE
//_____________________________________________________________________________



//_____________________________________________________________________________
// [SECTION] Header mess
//_____________________________________________________________________________
// Includes
#include <stddef.h>                 // NULL
#include <stdio.h>                  // NULL
#include <stdexcept>                // NULL
#include <cstring>                  // strchr, strncpy
#include <iostream>
#include <fstream>
#include <string>
#include <nlohmann/json.hpp>
#include <filesystem>
#include <vector>
#include <unordered_map>
#include <unistd.h>
#include <sys/stat.h>


namespace fs = std::filesystem;

#ifdef USE_REGISTERING
#include <chrono>
#endif // USE_REGISTERING

#ifdef USE_HSP
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/aes.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#endif


#ifndef VORTEX_API
#define VORTEX_API
#endif
#ifndef VORTEX_EXT_API
#define VORTEX_EXT_API              VORTEX_API
#endif


// Helper Macros
#ifndef VX_ASSERT
#include <assert.h>
#define VX_ASSERT(_EXPR)            assert(_EXPR)                               // You can override the default assert handler by editing vortex_configs.h
#endif
#define VX_ARRAYSIZE(_ARR)          ((int)(sizeof(_ARR) / sizeof(*(_ARR))))     // Size of a static C-style array. Don't use on pointers!
#define VX_UNUSED(_VAR)             ((void)(_VAR))                              // Used to silence "unused variable warnings". Often useful as asserts may be stripped out from final builds.
#define VX_OFFSETOF(_TYPE,_MEMBER)  offsetof(_TYPE, _MEMBER)                    // Offset of _MEMBER within _TYPE. Standardized as offsetof() in C++11
#define VX_CHECKVERSION()           VortexMaker::DebugCheckVersionAndDataLayout(VORTEX_VERSION) // Version


// Disable some of MSVC most aggressive Debug runtime checks in function header/footer (used in some simple/low-level functions)
#if defined(_MSC_VER) && !defined(__clang__)  && !defined(__INTEL_COMPILER) && !defined(IMGUI_DEBUG_PARANOID)
#define IM_MSVC_RUNTIME_CHECKS_OFF      __pragma(runtime_checks("",off))     __pragma(check_stack(off)) __pragma(strict_gs_check(push,off))
#define IM_MSVC_RUNTIME_CHECKS_RESTORE  __pragma(runtime_checks("",restore)) __pragma(check_stack())    __pragma(strict_gs_check(pop))
#else
#define IM_MSVC_RUNTIME_CHECKS_OFF
#define IM_MSVC_RUNTIME_CHECKS_RESTORE
#endif

// Warnings
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable: 26495)    // [Static Analyzer] Variable 'XXX' is uninitialized. Always initialize a member variable (type.6).
#endif
#if defined(__clang__)
#pragma clang diagnostic push
#if __has_warning("-Wunknown-warning-option")
#pragma clang diagnostic ignored "-Wunknown-warning-option"         // warning: unknown warning group 'xxx'
#endif
#pragma clang diagnostic ignored "-Wunknown-pragmas"                // warning: unknown warning group 'xxx'
#pragma clang diagnostic ignored "-Wold-style-cast"
#pragma clang diagnostic ignored "-Wfloat-equal"                    // warning: comparing floating point with == or != is unsafe
#pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"
#pragma clang diagnostic ignored "-Wreserved-identifier"            // warning: identifier '_Xxx' is reserved because it starts with '_' followed by a capital letter
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"          // warning: unknown option after '#pragma GCC diagnostic' kind
#pragma GCC diagnostic ignored "-Wclass-memaccess"  // [__GNUC__ >= 8] warning: 'memset/memcpy' clearing/writing an object of type 'xxxx' with no trivial copy-assignment; use assignment or value-initialization instead
#endif

#ifndef USE_LOG
#define Vx_LOG(_LOG)
#endif

#ifdef USE_LOG
#include <iostream>
#define Vx_LOG(_LOG) std::cout << _LOG << std::endl;
#endif

//_____________________________________________________________________________


//_____________________________________________________________________________
// [SECTION] Forward declarations of structures
//_____________________________________________________________________________
template<typename T>
struct hVector;

template <typename K, typename V>
struct hMap;

struct hString;
struct hArgs;

struct CommandOutput;

// Internals (from vortex_internals.h)
struct VxContext;
//_____________________________________________________________________________


typedef int VortexMakerChannel_Props;
typedef int VortexMakerMatrix_Props;


// Callback and functions types
typedef void*   (*VortexMakerMemAllocFunc)(size_t sz, void* user_data);               // Function signature for VortexMaker::SetAllocatorFunctions()
typedef void    (*VortexMakerMemFreeFunc)(void* ptr, void* user_data);                // Function signature for VortexMaker::SetAllocatorFunctions()



//_____________________________________________________________________________
// [SECTION] VortexMaker end-use API functions
//_____________________________________________________________________________
// Note for all VortexMaker internals modifications ! : Please considere 
// (Note that VortexMaker:: being a namespace, you can add extra VortexMaker:: functions 
// in your own separate file. Please don't modify VortexMaker source files!)
//_____________________________________________________________________________
namespace VortexMaker
{
    VORTEX_API VxContext*       CreateContext();
    VORTEX_API void             DestroyContext(VxContext* context = NULL);
    VORTEX_API VxContext*       GetCurrentContext();
    VORTEX_API void             SetCurrentContext(VxContext* context);
    VORTEX_API void             Initialize();

    VORTEX_API void             InitProject(nlohmann::json main_config);

    VORTEX_API void             RefreshDistToolchains();
    VORTEX_API void             RefreshDistHosts();

    bool                        DebugCheckVersionAndDataLayout(const char* version);


    VORTEX_API void             MoveAllContent();
    VORTEX_API void             CopyAllContent();
    VORTEX_API void             ExecuteCommand();
    VORTEX_API std::vector<std::string> SearchFiles(const std::string& path, const std::string& filename);

    VORTEX_API nlohmann::json DumpJSON(const std::string& file);


    // Memory Allocators
    // - Those functions are not reliant on the current context.
    // - DLL users: heaps and globals are not shared across DLL boundaries! You will need to call SetCurrentContext() + SetAllocatorFunctions()
    //   for each static/DLL boundary you are calling from. Read "Context and Memory Allocators" section of VortexMaker.cpp for more details.
    VORTEX_API void             SetAllocatorFunctions(VortexMakerMemAllocFunc alloc_func, VortexMakerMemFreeFunc free_func, void* user_data = NULL);
    VORTEX_API void             GetAllocatorFunctions(VortexMakerMemAllocFunc* p_alloc_func, VortexMakerMemFreeFunc* p_free_func, void** p_user_data);
    VORTEX_API void*            MemAlloc(size_t size);
    VORTEX_API void             MemFree(void* ptr);
    
} // namespace VortexMaker
//_____________________________________________________________________________



struct VxNewWrapper {};
inline void* operator new(size_t, VxNewWrapper, void* ptr) { return ptr; }
inline void  operator delete(void*, VxNewWrapper, void*)   {} // This is only required so we can use the symmetrical new()
#define VX_ALLOC(_SIZE)                     VortexMaker::MemAlloc(_SIZE)
#define VX_FREE(_PTR)                       VortexMaker::MemFree(_PTR)
#define VX_PLACEMENT_NEW(_PTR)              new(VxNewWrapper(), _PTR)
#define VX_NEW(_TYPE)                       new(VxNewWrapper(), VortexMaker::MemAlloc(sizeof(_TYPE))) _TYPE
template<typename T> void VX_DELETE(T* p)   { if (p) { p->~T(); VortexMaker::MemFree(p); } }

//=============================================================================
// hVector<>
// Lightweight hVector<>-like class to avoid dragging dependencies (also, some implementations of STL with debug enabled are absurdly slow, we bypass it so our code runs fast in debug).
//=============================================================================
// - You generally do NOT need to care or use this ever. But we need to make it available in imgui.h because some of our public structures are relying on it.
// - We use std-like naming convention here, which is a little unusual for this codebase.
// - Important: clear() frees memory, resize(0) keep the allocated buffer. We use resize(0) a lot to intentionally recycle allocated buffers across frames and amortize our costs.
// - Important: our implementation does NOT call C++ constructors/destructors, we treat everything as raw data! This is intentional but be extra mindful of that,
//   Do NOT use this class as a std::vector replacement in your own code! Many of the structures used by dear imgui can be safely initialized by a zero-memset.
//=============================================================================

template<typename T>
struct hVector
{
    int                 Size;
    int                 Capacity;
    T*                  Data;

    // Provide standard typedefs but we don't use them ourselves.
    typedef T                   value_type;
    typedef value_type*         iterator;
    typedef const value_type*   const_iterator;

    // Constructors, destructor 
    inline hVector(std::initializer_list<T> initList) : Size(0), Capacity(0), Data(nullptr) {reserve(initList.size());for (const T& value : initList) {new (&Data[Size++]) T(value);}}
    inline hVector()                                        { Size = Capacity = 0; Data = NULL; }
    inline hVector(const hVector<T>& src)                   { Size = Capacity = 0; Data = NULL; operator=(src); }
    inline hVector<T>& operator=(const hVector<T>& src)     { clear(); resize(src.Size); if (src.Data) memcpy(Data, src.Data, (size_t)Size * sizeof(T)); return *this; }
    inline ~hVector()                                       { if (Data) VX_FREE(Data); } // Important: does not destruct anything

    inline void         clear()                             { if (Data) { Size = Capacity = 0; VX_FREE(Data); Data = NULL; } }  // Important: does not destruct anything
    inline void         clear_delete()                      { for (int n = 0; n < Size; n++) IM_DELETE(Data[n]); clear(); }     // Important: never called automatically! always explicit.
    inline void         clear_destruct()                    { for (int n = 0; n < Size; n++){Data[n].~T();}clear();}

    inline bool         empty() const                       { return Size == 0; }
    inline int          size() const                        { return Size; }
    inline int          size_in_bytes() const               { return Size * (int)sizeof(T); }
    inline int          max_size() const                    { return 0x7FFFFFFF / (int)sizeof(T); }
    inline int          capacity() const                    { return Capacity; }
    inline T&           operator[](int i)                   { VX_ASSERT(i >= 0 && i < Size); return Data[i]; }
    inline const T&     operator[](int i) const             { VX_ASSERT(i >= 0 && i < Size); return Data[i]; }

    inline T*           begin()                             { return Data; }
    inline const T*     begin() const                       { return Data; }
    inline T*           end()                               { return Data + Size; }
    inline const T*     end() const                         { return Data + Size; }
    inline T&           front()                             { VX_ASSERT(Size > 0); return Data[0]; }
    inline const T&     front() const                       { VX_ASSERT(Size > 0); return Data[0]; }
    inline T&           back()                              { VX_ASSERT(Size > 0); return Data[Size - 1]; }
    inline const T&     back() const                        { VX_ASSERT(Size > 0); return Data[Size - 1]; }
    inline void         swap(hVector<T>& rhs)               { int rhs_size = rhs.Size; rhs.Size = Size; Size = rhs_size; int rhs_cap = rhs.Capacity; rhs.Capacity = Capacity; Capacity = rhs_cap; T* rhs_data = rhs.Data; rhs.Data = Data; Data = rhs_data; }

    inline int          _grow_capacity(int sz) const        { int new_capacity = Capacity ? (Capacity + Capacity / 2) : 8; return new_capacity > sz ? new_capacity : sz; }
    inline void         resize(int new_size)                { if (new_size > Capacity) reserve(_grow_capacity(new_size)); Size = new_size; }
    
    inline void         resize(int new_size, const T& v)    { if (new_size > Capacity)reserve(_grow_capacity(new_size));if (new_size > Size){for (int n = Size; n < new_size; n++){new (&Data[n]) T(v); }}Size = new_size;} 
    inline void         shrink(int new_size)                { VX_ASSERT(new_size <= Size); Size = new_size; } // Resize a vector to a smaller size, guaranteed not to cause a reallocation
    inline void         reserve(int new_capacity)           { if (new_capacity <= Capacity) return; T* new_data = (T*)VX_ALLOC((size_t)new_capacity * sizeof(T)); if (Data) { memcpy(new_data, Data, (size_t)Size * sizeof(T)); VX_FREE(Data); } Data = new_data; Capacity = new_capacity; }
    inline void         reserve_discard(int new_capacity)   { if (new_capacity <= Capacity) return; if (Data) VX_FREE(Data); Data = (T*)VX_ALLOC((size_t)new_capacity * sizeof(T)); Capacity = new_capacity; }

    // NB: It is illegal to call push_back/push_front/insert with a reference pointing inside the hVector data itself! e.g. v.push_back(v[10]) is forbidden.
    inline void         push_back(const T& v)               {if (Size == Capacity)reserve(_grow_capacity(Size + 1));new (&Data[Size]) T(v);Size++;}
    inline void         pop_back()                          { VX_ASSERT(Size > 0); Size--; }
    inline void         push_front(const T& v)              { if (Size == 0) push_back(v); else insert(Data, v); }
    
    inline T*           erase(const T* it)                  { VX_ASSERT(it >= Data && it < Data + Size);const ptrdiff_t off = it - Data;Data[off].~T();memmove(Data + off, Data + off + 1, ((size_t)Size - (size_t)off - 1) * sizeof(T));Size--;return Data + off;} 
    inline T*           erase(const T* it, const T* it_last){ VX_ASSERT(it >= Data && it < Data + Size && it_last >= it && it_last <= Data + Size); const ptrdiff_t count = it_last - it; const ptrdiff_t off = it - Data; memmove(Data + off, Data + off + count, ((size_t)Size - (size_t)off - (size_t)count) * sizeof(T)); Size -= (int)count; return Data + off; }
    inline T*           erase_unsorted(const T* it)         { VX_ASSERT(it >= Data && it < Data + Size);  const ptrdiff_t off = it - Data; if (it < Data + Size - 1) memcpy(Data + off, Data + Size - 1, sizeof(T)); Size--; return Data + off; }
    inline T*           insert(const T* it, const T& v)     { VX_ASSERT(it >= Data && it <= Data + Size); const ptrdiff_t off = it - Data; if (Size == Capacity) reserve(_grow_capacity(Size + 1)); if (off < (int)Size) memmove(Data + off + 1, Data + off, ((size_t)Size - (size_t)off) * sizeof(T)); memcpy(&Data[off], &v, sizeof(v)); Size++; return Data + off; }
    inline bool         contains(const T& v) const          { const T* data = Data;  const T* data_end = Data + Size; while (data < data_end) if (*data++ == v) return true; return false; }
    inline T*           find(const T& v)                    { T* data = Data;  const T* data_end = Data + Size; while (data < data_end) if (*data == v) break; else ++data; return data; }
    inline const T*     find(const T& v) const              { const T* data = Data;  const T* data_end = Data + Size; while (data < data_end) if (*data == v) break; else ++data; return data; }
    inline int          find_index(const T& v) const        { const T* data_end = Data + Size; const T* it = find(v); if (it == data_end) return -1; const ptrdiff_t off = it - Data; return (int)off; }
    inline bool         find_erase(const T& v)              { const T* it = find(v); if (it < Data + Size) { erase(it); return true; } return false; }
    inline bool         find_erase_unsorted(const T& v)     { const T* it = find(v); if (it < Data + Size) { erase_unsorted(it); return true; } return false; }
    inline int          index_from_ptr(const T* it) const   { VX_ASSERT(it >= Data && it < Data + Size); const ptrdiff_t off = it - Data; return (int)off; }
};
//=============================================================================

//=============================================================================
// hMap<>
//=============================================================================
template <typename K, typename V>
class hMap 
{
private:
    struct KeyValuePair {K key;V value;};
    hVector<KeyValuePair> data;

public:
    inline void         insert(const K& key, const V& value){ for (int i = 0; i < data.size(); ++i) {if (data[i].key == key) {data[i].value = value;return;}}KeyValuePair pair;pair.key = key;pair.value = value;data.push_back(pair);}
    inline bool         find(const K& key, V& value) const  { for (int i = 0; i < data.size(); ++i) {if (data[i].key == key) {value = data[i].value;return true;}}return false;}
    inline bool         remove(const K& key)                { for (int i = 0; i < data.size(); ++i) {if (data[i].key == key) {data.erase(data.begin() + i);return true;}}return false;}
    inline int          size() const                        { return data.size(); }
    inline const V&     find(const K& key) const            {for (int i = 0; i < data.size(); ++i) {if (data[i].key == key) {return data[i].value;}}throw std::out_of_range("Key not found in hMap");}
    inline const V&     at(const K& key) const              {for (int i = 0; i < data.size(); ++i) {if (data[i].key == key) {return data[i].value;}}throw std::out_of_range("Key not found in hMap");}

    bool                operator!=(const K& key) const      {for (int i = 0; i < data.size(); ++i) {if (data[i].key == key) {return false;}}return true;}
    V&                  operator[](const K& key)            {for (int i = 0; i < data.size(); ++i) {if (data[i].key == key) {return data[i].value;}}KeyValuePair pair;pair.key = key;data.push_back(pair);return data.back().value;}

    typename            hVector<KeyValuePair>::iterator        begin() { return data.begin();}
    typename            hVector<KeyValuePair>::iterator        end()   { return data.end();}
    typename            hVector<KeyValuePair>::const_iterator  begin() const {return data.begin();}
    typename            hVector<KeyValuePair>::const_iterator  end() const {return data.end();}
};
//=============================================================================

constexpr unsigned int str2int(const char *str, int h = 0) {
  return !str[h] ? 5381 : (str2int(str, h + 1) * 33) ^ str[h];
}

static long stringtoint(const char *s)
{
    long i;
    i = 0;
    while(*s >= '0' && *s <= '9')
    {
        i = i * 10 + (*s - '0');
        s++;
    }
    return i;
}


//=============================================================================
// hString<>
//=============================================================================
struct hString
{
    hVector<char> Buf;
    VORTEX_API static char EmptyString[1];

    hString() : Data(nullptr), Size(0), Capacity(0) { }
    hString(const char* str) {this->append(str);}
    hString(int intValue) {char buffer[20];snprintf(buffer, sizeof(buffer), "%d", intValue);this->append(buffer);}

    size_t              find(const char* substr, size_t start = 0) const        {if (Buf.Data == nullptr || substr == nullptr || start >= Buf.Size) {return npos;}const char* result = std::strstr(Buf.Data + start, substr);if (result == nullptr) {return npos;}return result - Buf.Data;}
    size_t              rfind(const char* substr, size_t start = npos) const    {if (Buf.Data == nullptr || substr == nullptr) {return npos;}size_t searchStart = (start == npos) ? Size - 1 : std::min(start, Size - 1);for (size_t i = searchStart; i < Size; --i) {const char* result = std::strstr(Buf.Data + i, substr);if (result != nullptr) {return i;}}return npos;}
    hString             substr(size_t pos, size_t len = npos) const             {if (Buf.Data == nullptr || pos >= Buf.Size) {return hString();}if (len == npos || pos + len > Buf.Size) {len = Buf.Size - pos;}hString result;result.reserve(static_cast<int>(len));result.append(Buf.Data + pos, Buf.Data + pos + len);return result;}
    size_t              find_first_of(const char* charsToFind, size_t start = 0) const {if (Data == nullptr || charsToFind == nullptr || start >= Size) {return npos;}for (size_t i = start; i < Size; ++i) {char c = Data[i];if (strchr(charsToFind, c) != nullptr) {return i;}}return npos; }
    int                 length() const                                          {return static_cast<int>(Size);}
    void                push_back(char c)                                       {Buf.push_back(c);}
    const char*         begin() const { return Buf.Data ? &Buf.front() : EmptyString; }
    const char*         end() const { return Buf.Data ? &Buf.back() : EmptyString; }
    int                 size() const { return Buf.Size ? Buf.Size - 1 : 0; }
    bool                empty() const { return Buf.Size <= 1; }
    void                reserve(int capacity) {if (capacity <= Capacity) {return;}char* newData = new char[capacity];if (Data) {memcpy(newData, Data, Size);delete[] Data;}Data = newData;Capacity = capacity;}
    void                clear() {if (Data) {delete[] Data;Data = nullptr;Size = 0;Capacity = 0;}}
    const char*         c_str() const { return Buf.Data ? Buf.Data : EmptyString; }

    VORTEX_API void   append(const char* str, const char* str_end = NULL);

    inline char         operator[](int i) const { VX_ASSERT(Buf.Data != NULL); return Buf.Data[i]; }
    bool                operator==(const char* str) const {if (Buf.Data == nullptr && str == nullptr) {return true; }if (Buf.Data == nullptr || str == nullptr) {return false; }return strcmp(Buf.Data, str) == 0;}
    bool                operator==(const hString& other) const {return operator==(other.c_str());}
    hString&            operator=(const char* str) {this->clear();this->append(str);return *this;}
    hString&            operator+=(int value) {char strValue[12]; snprintf(strValue, sizeof(strValue), "%d", value);this->append(strValue);return *this;}
    hString&            operator+=(const char* str) {this->append(str);return *this;}
    hString&            operator+=(const hString& other) {this->append(other.c_str());return *this;}
    bool                operator!=(const char* rhs) const {return strcmp(Data, rhs) != 0;}
    friend hString      operator+(const char* lhs, const hString& rhs) {hString result(lhs);result.append(rhs.c_str());return result;}
    friend hString      operator+(const hString& lhs, const char* rhs) {hString result(lhs);result.append(rhs);return result;}
    friend hString      operator+(const hString& lhs, const hString& rhs) {hString result(lhs);result.append(rhs.c_str());return result;}

    static const size_t npos = static_cast<size_t>(-1);
    char* Data;
    size_t Size;
    size_t Capacity;

};
//=============================================================================

//=============================================================================
//  h64
//=============================================================================
struct h64
{
    hString encode;
    hString decode;
};

//=============================================================================

#include <chrono>
struct hTime
{
    static hString GetCurrentTimestamp();
};


//=============================================================================
//  h64
//=============================================================================
struct hEncrypt
{
    
};

//=============================================================================

//=============================================================================
//  hArgs
//=============================================================================
class hArgs {
public:
    template<typename T> void add(const hString& tag, T value) {arguments[tag] = new ArgumentHolder<T>(value); registered_arguments.push_back(tag);}
    template<typename T> T get(const hString& tag, const T& defaultT) const {if(registered_arguments.contains(tag)){auto it = arguments.find(tag); return dynamic_cast<ArgumentHolder<T>*>(it)->getValue();}return defaultT;}

    ~hArgs() {for (const auto& pair : arguments) {delete pair.value;}}

private:
    class ArgumentBase {public: virtual ~ArgumentBase() = default;};

    template<typename T> 
    class ArgumentHolder : public ArgumentBase {
    public:
        ArgumentHolder(T value) : storedValue(value) {}
        T getValue() const { return storedValue;}
    private:
        T storedValue;
    };

    hVector<hString> registered_arguments;
    hMap<hString, ArgumentBase*> arguments;
};
//=============================================================================

//_____________________________________________________________________________
// [SECTION] Structures
//_____________________________________________________________________________

struct VxPackageInterface{
    std::string emplacement;
    std::string label;
    bool resolved;
};



struct VXPackage_Action{
    int         priority;
    std::string type;
    std::string emplacement;
    std::string command;
    std::string executionSequence;
};


struct VxPackageCompilation{
    bool useCompilationOptimization = false; // For a fully custom behavior

    std::string customScript = "unknow"; // For a fully custom behavior
    std::string configurationCommand = "unknow"; // Final cooked configuration command (Step 1 of vortex basic  compilation)
    std::string compilationCommand = "unknow"; // Final cooked compilation command (Step 2 of vortex basic  compilation)
    std::string installationCommand = "unknow"; // Final cooked installation command (Step 3 of vortex basic compilation)
    std::vector<std::pair<std::string, std::string>> configurationParams; // previous command' parameters
    std::vector<std::pair<std::string, std::string>> compilationParams; // previous command' parameters
    std::vector<std::pair<std::string, std::string>> installationParams; // previous command' parameters

    std::vector<std::pair<std::string, std::string>> configurationSuffixes;
    std::vector<std::pair<std::string, std::string>> configurationParameters;

    std::vector<std::pair<std::string, std::string>> compilationSuffixes;
    std::vector<std::pair<std::string, std::string>> compilationParameters;

    std::vector<std::pair<std::string, std::string>> installationSuffixes;
    std::vector<std::pair<std::string, std::string>> installationParameters;

    // Configuration Commands
    std::string customConfigurationProcess = "not specified"; // Customized configuration processus
    std::string exclusiveCustomConfigProcess = "not specified"; // Absolute customized configuration processus

    // Compilation Commands
    std::string customCompilationProcess = "not specified";
    std::string exclusiveCustomCompilationProcess = "not specified";

    // Installation Commands
    std::string customInstallationProcess = "not specified";
    std::string exclusiveCustomInstallationProcess = "not specified";
};


struct VxDiag{
    std::string properName;
    std::string statut;
    std::string description;
    std::string output;
    int code;
};

struct VxPackage{

    std::string name = "unknow"; // Proper name of the package
    std::string path = "unknow"; // Path to package
    std::string distPath = "unknow"; // Path to package
    std::string fileName = "unknow"; // Path to package
    std::string compressed = "unknow"; // Path to package
    std::string description = "unknow";  // Short description of the package
    std::string label = "unknow"; // Dedicated package name (unique)
    std::string format = "unknow"; // Format of the package
    int         priority = -1;  // Priority of process when the package is in a queue
    std::vector<std::string> archs; // CPU archs supported by package
    std::vector<std::string> scopes; // Where the package can be used. (in a Toolchain, in a Host, Final system, etc...)
    std::string locality = "unknow"; // local (in the component), global (in the project), public (in a repository)
    std::string installationMethod = "unknow"; // compilation, installation
    bool        enabled = false; 
    std::string strapper = "unknow";
    std::string type = "unknow";

    VxPackageCompilation compilation;
    // Package manager profile

    std::unordered_map<std::string, std::shared_ptr<VxDiag>> diags;

    hVector<std::shared_ptr<VXPackage_Action>> actions;

    void ExecuteActions(std::string sequence, std::shared_ptr<VxPackage> package);

    void AddDiag(std::string name) {
        std::shared_ptr<VxDiag> diag = std::make_shared<VxDiag>();
        diag->code = 999;
        diag->output = "No output.";
        this->diags[name] = diag;
    };

    void SetDiagCode(std::string name, int code) {
          auto it = diags.find(name);
    if (it != diags.end()) {
        it->second->code = code;
    } else {
        std::cerr << "Key not finded" << std::endl;
    }
    };


    void SetDiagOutput(std::string name, std::string output) {
          auto it = diags.find(name);
    if (it != diags.end()) {
        it->second->output = output;
    } else {
        std::cerr << "Key not finded" << std::endl;
    }
    };


    
    std::string GetDiagStatus(std::string name) {};

    std::string GetDiagOutput(std::string name) {
        auto it = diags.find(name);
    if (it != diags.end()) {
        std::shared_ptr<VxDiag> foundDiag = it->second;
        return foundDiag->output;
    } else {
        return "No found.";
        std::cerr << "Key not finded" << std::endl;
    }

    };

    int GetDiagCode(std::string name) {
        auto it = diags.find(name);
    if (it != diags.end()) {
        std::shared_ptr<VxDiag> foundDiag = it->second;
        return foundDiag->code;
    } else {
        return -999;
        std::cerr << "Key not finded" << std::endl;
    }
    };

    

};

struct VxToolchain{
    // Vortex project informations
    std::string name;
    std::string author;
    std::string description;
    std::string type;
    std::string state;
    std::string vendor;
    std::string platform;

    std::string target_arch;
    std::string builder_arch;
    std::string host_arch;


    // Low level toolchain informations
    std::string workingPath             = "not specified";
    std::string ouputPath              = "not specified";
    std::string toolchainSourcePath     = "not specified";
    bool        useWorkingPath          = false;
    bool        isCompressed            = false;
    std::string compressionMode         = "not specified";


    std::string localPackagesPath;

    

        // -> Toolchain Sysroot Path
    std::string sysrootPath             = "not specified";
    std::string debugrootPath             = "not specified";
    std::string vxToolchain_TempPackagesPoolPath             = "not specified";
    std::string crosstoolsPath             = "not specified";
    std::string distPackagesPath             = "not specified";
    std::string currentBuildArch = "unknow";
    std::string vxToolchain_HostPath            = "not specified";
    std::string vxToolchain_ProdPath            = "not specified";

        // -> Source pool
    std::string vxToolchain_SourcesPool             = "not specified";

        // -> Temporary source directory in toolchain
    std::string vxToolchain_SourcesToolchain        = "not specified";

        // -> Complete triplet to handle or complete if specified
    std::string hostTriplet             = "not specified";
    std::string builderTriplet          = "not specified";
    std::string targetTriplet           = "not specified";

        // -> Simple ARCH index
    std::string vxToolchain_BuilderArch             = "not specified";
    std::string vxToolchain_TargetArch              = "not specified";
    std::string vxToolchain_HostArch                = "not specified";

    // Vector de packages

    hVector<std::shared_ptr<VxPackageInterface>> registeredPackages;
    hVector<std::shared_ptr<VxPackage>> packages;
    // Scripts
    // Modules & other assets..
    // Patchs


    std::string GetTriplet(std::string triplet_type);

    void PreBuild();
    void Build();
    void PostBuild();

    void RegisterPackage(const std::string label,const std::string emplacemement){
        std::shared_ptr<VxPackageInterface> newPackageInterface = std::make_shared<VxPackageInterface>();
        newPackageInterface->label = label;
        newPackageInterface->emplacement = emplacemement;
        newPackageInterface->resolved = false;
        registeredPackages.push_back(newPackageInterface);
    }

    void FindPackages();

    void PreparePackage(std::string packageName);
    void ConfigurePackage(std::string packageName);
    void CompilePackage(std::string packageName);
    void InstallPackage(std::string packageName);

    void CreateToolchainDirectory(/*VxDirectory*/);
}; 


struct VxHost{
    // Vortex project informations
    std::string name;
    std::string author;
    std::string description;
    std::string type;
    std::string state;
    std::string vendor;
    std::string platform;

    std::string toolchainToUse;

    std::string distPackagesPath             = "not specified";

    std::string localPackagesPath;

    std::string target_arch;
    std::string builder_arch;
    std::string host_arch;

    std::string host          = "not specified";

    std::string hostTriplet             = "not specified";
    std::string builderTriplet          = "not specified";
    std::string targetTriplet           = "not specified";

    std::string GetTriplet(std::string triplet_type);

    hVector<std::shared_ptr<VxPackageInterface>> registeredPackages;
    hVector<std::shared_ptr<VxPackage>> packages;


    void RegisterPackage(const std::string label,const std::string emplacemement){
        std::shared_ptr<VxPackageInterface> newPackageInterface = std::make_shared<VxPackageInterface>();
        newPackageInterface->label = label;
        newPackageInterface->emplacement = emplacemement;
        newPackageInterface->resolved = false;
        registeredPackages.push_back(newPackageInterface);
    }

    void FindPackages();

    void PreBuild();
    void Build();
    void PostBuild();

};


struct VxDistToolchain{
    // Vortex project informations
    std::string name;
    std::string author;
    std::string target;
    std::string description;
    std::string type;
    std::string state;
    std::string path;
    std::string vendor;
    std::string platform;


    std::string CC;
    std::string CXX;
    std::string AR;
    std::string AS;
    std::string RANLIB;
    std::string LD;
    std::string STRIP;
    
};

struct VxDistHost{
    // Vortex project informations
    std::string name;
    std::string author;
    std::string target;
    std::string description;
    std::string type;
    std::string path;
    std::string state;
    std::string vendor;
    std::string platform;

    std::string CC;
    std::string CXX;
    std::string AR;
    std::string AS;
    std::string RANLIB;
    std::string LD;
    std::string STRIP;

    void TestingChroot();
};


//_____________________________________________________________________________




#endif // #ifndef VORTEX_DISABLE