//
//  crash_writer.hpp
//  Core dumper API for Microsoft Windows
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#pragma once

#ifdef _WIN32
#include <windows.h>
#include <dbghelp.h>
#include <psapi.h>

#pragma comment(lib, "dbghelp.lib")
#pragma comment(lib, "psapi.lib")

#include <string>
#include <cstdio>
#include <cstdarg>
#ifndef SymLoadModuleExA
#define SymLoadModuleExA SymLoadModuleEx
#endif
namespace vxe::crash {

namespace detail {

inline bool g_sym_initialized = false;
inline std::string g_session_id;

inline void write_str(HANDLE file, const char* str)
{
    if (!str) return;
    DWORD written = 0;
    WriteFile(file, str, (DWORD)strlen(str), &written, nullptr);
}

inline void write_fmt(HANDLE file, const char* fmt, ...)
{
    char buffer[4096];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
    write_str(file, buffer);
}

inline void register_module(HMODULE module)
{
    HANDLE process = GetCurrentProcess();

    char path[MAX_PATH];
    GetModuleFileNameA(module, path, MAX_PATH);

    std::string dir(path);
    size_t pos = dir.find_last_of("\\/");
    std::string dll_dir = (pos != std::string::npos) ? dir.substr(0, pos) : ".";

    char current_search_path[4096] = {0};
    SymGetSearchPath(process, current_search_path, sizeof(current_search_path));

    std::string search_path(current_search_path);
    if (search_path.find(dll_dir) == std::string::npos)
    {
        search_path += ";" + dll_dir;
        SymSetSearchPath(process, search_path.c_str());
    }

    DWORD64 base = SymLoadModuleExA(
        process,
        nullptr,
        path,
        nullptr,
        (DWORD64)module,
        0,
        nullptr,
        0
    );

    IMAGEHLP_MODULE64 mod{};
    mod.SizeOfStruct = sizeof(mod);
    if (SymGetModuleInfo64(process, base ? base : (DWORD64)module, &mod))
    {
        char dbg[512];
        snprintf(dbg, sizeof(dbg),
            "[CrashWriter] Module: %s | PDB: %s | SymType: %d\n",
            mod.ModuleName,
            mod.LoadedPdbName,
            (int)mod.SymType);
        OutputDebugStringA(dbg);
    }
}

inline void init_symbols()
{
    if (g_sym_initialized)
        return;

    HANDLE process = GetCurrentProcess();

    SymSetOptions(
        SYMOPT_UNDNAME |
        SYMOPT_DEFERRED_LOADS |
        SYMOPT_LOAD_LINES |
        SYMOPT_AUTO_PUBLICS |
        SYMOPT_ALLOW_ABSOLUTE_SYMBOLS |
        SYMOPT_DEBUG
    );

    if (!SymInitialize(process, nullptr, TRUE))
        return;

    char path[MAX_PATH];
    GetModuleFileNameA(nullptr, path, MAX_PATH);

    std::string dir(path);
    size_t pos = dir.find_last_of("\\/");
    if (pos != std::string::npos)
        dir = dir.substr(0, pos);

    SymSetSearchPath(process, dir.c_str());

    g_sym_initialized = true;
}

inline void load_modules()
{
    HANDLE process = GetCurrentProcess();

    HMODULE mods[1024];
    DWORD cbNeeded = 0;

    if (!EnumProcessModules(process, mods, sizeof(mods), &cbNeeded))
        return;

    for (DWORD i = 0; i < cbNeeded / sizeof(HMODULE); i++)
    {
        char name[MAX_PATH] = {0};
        if (GetModuleFileNameA(mods[i], name, MAX_PATH))
        {
            SymLoadModuleExA(
                process,
                nullptr,
                name,
                nullptr,
                (DWORD64)mods[i],
                0,
                nullptr,
                0
            );
        }
    }

    SymRefreshModuleList(process);
}

inline void build_crash_path(char* out, const char* filename)
{
    const char* profile = getenv("USERPROFILE");
    if (!profile) profile = ".";

    snprintf(
        out,
        MAX_PATH,
        "%s\\.vx\\sessions\\%s\\crash\\%s",
        profile,
        g_session_id.c_str(),
        filename
    );
}

inline void write_callstack(
    HANDLE file,
    HANDLE process,
    HANDLE thread,
    CONTEXT* ctx_in)
{
    CONTEXT ctx = *ctx_in;

    load_modules();

    STACKFRAME64 frame{};
    DWORD machine = 0;

#ifdef _M_X64
    machine = IMAGE_FILE_MACHINE_AMD64;
    frame.AddrPC.Offset    = ctx.Rip;
    frame.AddrPC.Mode      = AddrModeFlat;
    frame.AddrFrame.Offset = ctx.Rsp;
    frame.AddrFrame.Mode   = AddrModeFlat;
    frame.AddrStack.Offset = ctx.Rsp;
    frame.AddrStack.Mode   = AddrModeFlat;
#else
    machine = IMAGE_FILE_MACHINE_I386;
    frame.AddrPC.Offset    = ctx.Eip;
    frame.AddrPC.Mode      = AddrModeFlat;
    frame.AddrFrame.Offset = ctx.Ebp;
    frame.AddrFrame.Mode   = AddrModeFlat;
    frame.AddrStack.Offset = ctx.Esp;
    frame.AddrStack.Mode   = AddrModeFlat;
#endif

    write_str(file, "\nCallstack:\n");
    write_str(file, "--------------------------------------------------------------------------------\n");

    char sym_buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME];
    memset(sym_buffer, 0, sizeof(sym_buffer));

    SYMBOL_INFO* symbol = (SYMBOL_INFO*)sym_buffer;
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
    symbol->MaxNameLen   = MAX_SYM_NAME;

    IMAGEHLP_LINE64 line{};
    line.SizeOfStruct = sizeof(line);

    for (int i = 0; i < 64; i++)
    {
        if (!StackWalk64(
            machine,
            process,
            thread,
            &frame,
            &ctx,
            nullptr,
            SymFunctionTableAccess64,
            SymGetModuleBase64,
            nullptr))
            break;

        if (!frame.AddrPC.Offset)
            break;

        DWORD64 addr = frame.AddrPC.Offset;

        const char* func = "??";
        DWORD64 displacement = 0;

        if (SymFromAddr(process, addr, &displacement, symbol))
            func = symbol->Name;

        char moduleName[256] = "?";
        IMAGEHLP_MODULE64 mod{};
        mod.SizeOfStruct = sizeof(mod);

        if (SymGetModuleInfo64(process, addr, &mod))
            strncpy_s(moduleName, mod.ModuleName, _TRUNCATE);

        write_fmt(file,
            "#%-2d  0x%016llX  %-40s (%s)",
            i, addr, func, moduleName);

        if (displacement)
            write_fmt(file, " +0x%llX", displacement);

        if (SymGetLineFromAddr64(process, addr, nullptr, &line))
        {
            write_fmt(file, "  [%s:%lu]",
                line.FileName ? line.FileName : "?",
                line.LineNumber);
        }

        write_str(file, "\n");
    }

    write_str(file, "--------------------------------------------------------------------------------\n");
}

inline void write_minidump(EXCEPTION_POINTERS* ep)
{
    char path[MAX_PATH];
    build_crash_path(path, "core_dumped.dmp");

    HANDLE file = CreateFileA(path, GENERIC_WRITE, 0, nullptr,
        CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

    if (file == INVALID_HANDLE_VALUE)
        return;

    MINIDUMP_EXCEPTION_INFORMATION mei{};
    mei.ThreadId = GetCurrentThreadId();
    mei.ExceptionPointers = ep;
    mei.ClientPointers = FALSE;

    MiniDumpWriteDump(
        GetCurrentProcess(),
        GetCurrentProcessId(),
        file,
        (MINIDUMP_TYPE)(
            MiniDumpWithIndirectlyReferencedMemory |
            MiniDumpWithDataSegs |
            MiniDumpWithThreadInfo |
            MiniDumpWithModuleHeaders |
            MiniDumpWithUnloadedModules |
            MiniDumpWithFullMemoryInfo
        ),
        &mei, nullptr, nullptr
    );

    CloseHandle(file);
}

inline LONG WINAPI UnhandledExceptionHandler(EXCEPTION_POINTERS* ep)
{
    char dir[MAX_PATH];
    build_crash_path(dir, "");
    CreateDirectoryA(dir, nullptr);

    char txt[MAX_PATH];
    build_crash_path(txt, "core_dumped.txt");

    HANDLE file = CreateFileA(txt, GENERIC_WRITE, FILE_SHARE_READ,
        nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

    if (file != INVALID_HANDLE_VALUE)
    {
        write_str(file, "=== Vortex Crash Report ===\n");

        SYSTEMTIME st;
        GetLocalTime(&st);

        write_fmt(file, "Time      : %04d-%02d-%02d %02d:%02d:%02d\n",
            st.wYear, st.wMonth, st.wDay,
            st.wHour, st.wMinute, st.wSecond);

        write_fmt(file, "Exception : 0x%08X\n",
            ep->ExceptionRecord->ExceptionCode);

        write_fmt(file, "Address   : 0x%p\n",
            ep->ExceptionRecord->ExceptionAddress);

        if (ep->ExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION)
        {
            const char* type =
                ep->ExceptionRecord->ExceptionInformation[0] == 1 ? "WRITE" : "READ";

            write_fmt(file, "Access    : %s at 0x%p\n",
                type, (void*)ep->ExceptionRecord->ExceptionInformation[1]);
        }

        write_callstack(file, GetCurrentProcess(), GetCurrentThread(), ep->ContextRecord);

        CloseHandle(file);
    }

    write_minidump(ep);

    return EXCEPTION_EXECUTE_HANDLER;
}

} // namespace detail

inline void install(const std::string& session_id)
{
    detail::g_session_id = session_id;
    detail::init_symbols();
    SetUnhandledExceptionFilter(detail::UnhandledExceptionHandler);
}

} // namespace vxe::crash

#else

namespace vxe::crash {
inline void install(const std::string&) {}
}

#endif