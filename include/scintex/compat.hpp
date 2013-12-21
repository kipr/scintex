#ifndef _SCINTEX_COMPAT_HPP_
#define _SCINTEX_COMPAT_HPP_

#ifdef WIN32
#ifdef scintex_EXPORTS
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT __declspec(dllimport)
#endif
#else
#define DLL_EXPORT
#endif

#endif