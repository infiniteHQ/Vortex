//
//  vortex_configs.h
//  Configurations of Vortex core
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//
#pragma once

#if defined(_WIN32) || defined(_WIN64)
#ifdef VORTEX_EXPORTS
#define VORTEX_API __declspec(dllexport)
#else
#define VORTEX_API __declspec(dllimport)
#endif
#endif