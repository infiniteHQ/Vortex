#pragma once

#if !defined(_WIN32) || !defined(_WIN64)
#ifdef VORTEX_EXPORTS
    #define VORTEX_API __declspec(dllexport)
#else
    #define VORTEX_API __declspec(dllimport)
#endif
#endif