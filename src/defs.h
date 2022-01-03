#ifndef LOX_DEFS_H
#define LOX_DEFS_H

#ifdef __cpp_exceptions
#define LOX_EXCEPTION_ENABLED
#endif

#ifdef NDEBUG
#define LOX_NOEXCEPT noexcept
#else
#define LOX_NOEXCEPT
#endif

#endif
