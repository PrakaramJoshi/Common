#ifndef ACE_MACROS_H
#define ACE_MACROS_H

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#include <sys/param.h>
#endif

// Packing directives....
#ifdef _MSC_VER
#define ACE_BEGIN_PACK __pragma( pack(push, 1) )
#define ACE_END_PACK __pragma( pack(pop) )
#define ACE_PACKED(DECLARATION) __pragma( pack(push, 1) ) DECLARATION __pragma( pack(pop) )
#define ACE_DEPRECATED(func) __declspec(deprecated) func
#define ACE_NOEXCEPT
#else
#define ACE_BEGIN_PACK 
#define ACE_END_PACK __attribute__((packed))
#define ACE_PACKED(DECLARATION) DECLARATION __attribute__((packed))
#define ACE_DEPRECATED(func) func __attribute__ ((deprecated))
#define ACE_NOEXCEPT noexcept
#endif

#endif