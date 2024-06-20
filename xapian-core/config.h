/* Disable stupid MSVC warnings. */
#ifdef _MSC_VER
/* Passing an empty parameter to a single parameter macro. */
# pragma warning(disable:4003)
/* Assignment in conditional expression; other compilers suppress with extra parentheses but not MSVC. */
# pragma warning(disable:4706)
/* A "performance" warning for converting int to bool. */
# pragma warning(disable:4800)
/* Character in string literal can't be represented in current code page. */
# pragma warning(disable:4566)

/* POSIX get to deprecate POSIX things, not Microsoft. */
# ifndef _CRT_NONSTDC_NO_WARNINGS
#  define _CRT_NONSTDC_NO_WARNINGS
# endif
# ifndef _CRT_SECURE_NO_WARNINGS
#  define _CRT_SECURE_NO_WARNINGS
# endif

/* WSAAddressToStringA() is deprecated and we should apparently use
 * WSAAddressToStringW(), but we don't want wide character output so
 * that would mean we'd have to convert the result back to ASCII
 * which is a really stupid idea.
 */
# ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#  define _WINSOCK_DEPRECATED_NO_WARNINGS
# endif

/* Tell MSVC we want M_PI, etc defined. */
# ifndef _USE_MATH_DEFINES
#  define _USE_MATH_DEFINES
# endif

/* Tell MSVC we don't want max() and min() macros defined. */
# ifndef NOMINMAX
#  define NOMINMAX
# endif
#endif

/* Tell zlib to include const in method signatures.  Define this in config.h
 * to ensure it always gets defined before zlib.h is included.
 */
#define ZLIB_CONST

/* Make the POSIX-like functions support large files.  MSVC needs this;
 * current mingw32 does too; mingw64 supports _FILE_OFFSET_BITS, which
 * AC_SYS_LARGEFILE should discover and enable automatically.
 */
#if defined _MSC_VER || \
    (defined __MINGW32__ && !defined _FILE_OFFSET_BITS)

# include <sys/stat.h>
# include <sys/types.h>
# include <io.h>
/* zlib.h uses off_t so we need to include it before we redefine off_t. */
//# include <zlib.h>

// The default stat() and fstat() use 32-bit filesizes and time_t, so we need
// to arrange to use 64-bit variants instead.

# ifdef stat
// Break #undef lines to stop config.status from commenting them out.
#  undef \
	stat
# endif

// This hack is a problem is we ever want a method called "stat".
# ifdef _MSC_VER
// MSVC needs to call _stat64() instead of stat() and the struct which holds
// the information is `struct _stat64` instead of `struct stat` so we just
// use #define to replace both in one go.
#  define stat _stat64
# else
// Mingw32 has _stat64() but unhelpfully for our purposes, the struct is
// called __stat64 (with an extra underscore).  We hack around this by
// defining stat to __stat64 which sorts out the struct, and then using
// a second macro to forward function-like uses of __stat64() to
// _stat64().
#  define stat __stat64
#  define __stat64(PATH, STATBUF) _stat64((PATH), (STATBUF))
# endif

// We also want to use _fstat64() instead of fstat() but in this case we can
// use a function-like macro, so we could have a method called fstat so long
// as it didn't take two parameters.

# ifdef fstat
// Break #undef lines to stop config.status from commenting them out.
#  undef \
	fstat
# endif

# define fstat(FD, BUF) _fstat64(FD, BUF)

# ifdef lseek
// Break #undef lines to stop config.status from commenting them out.
#  undef \
	lseek
# endif

# ifdef off_t
// Break #undef lines to stop config.status from commenting them out.
#  undef \
	off_t
# endif

# define lseek(FD, OFF, WHENCE) _lseeki64(FD, OFF, WHENCE)
/* Redefine via a typedef so C++ code off_t(-1) works - it wouldn't if we did:
 * #define off_t long long
 */
typedef long long off_t_redefinition_typedef;
# define off_t off_t_redefinition_typedef

#endif

/* MSVC defines _WIN32 but not __WIN32__. */
#if !defined __WIN32__ && defined _WIN32
# define __WIN32__
#endif

/* MSVC defines _WIN64 but not __WIN64__. */
#if !defined __WIN64__ && defined _WIN64
# define __WIN64__
#endif

/* Default to enabling _FORTIFY_SOURCE at level 2 for GCC-like compilers.  It
 * shouldn't cause a problem to define it where it's not supported.
 *
 * The FORTIFY_SOURCE_OK check avoids auto-enabling on some mingw-w64 versions
 * which required linking with -lssp too.  This was addressed in v11.0.0
 * (released 2023-04-28).
 *
 * Check if _FORTIFY_SOURCE is already defined to allow the user to override
 * our choice with "./configure CPPFLAGS=-D_FORTIFY_SOURCE=0" or "...=1" or
 * "...=3".
 */
#if defined FORTIFY_SOURCE_OK && !defined _FORTIFY_SOURCE
# define _FORTIFY_SOURCE 2
#endif

/* For compilers which support it (such as GCC, clang, Intel's C++ compiler)
 * we can use __builtin_expect to give the compiler hints about branch
 * prediction.  See HACKING for how to use these.
 */
#if HAVE_DECL___BUILTIN_EXPECT
/* The arguments of __builtin_expect() are both long, so use !! to ensure that
 * the first argument is always an integer expression, and always 0 or 1, but
 * still has the same truth value for the if or while it is used in.
 */
# define rare(COND) __builtin_expect(!!(COND), 0)
# define usual(COND) __builtin_expect(!!(COND), 1)
#else
# define rare(COND) (COND)
# define usual(COND) (COND)
#endif

/* This macro can be used around expressions which intentionally make use of
 * unsigned integer overflow (which is defined behaviour, unlike signed integer
 * overflow) so that we can use UBSan's optional unsigned-integer-overflow
 * check to find potential bugs where we unintentionally overflow an unsigned
 * integer.
 */
#ifdef __clang__
#define UNSIGNED_OVERFLOW_OK(X) \
    ([&]() __attribute__((no_sanitize("unsigned-integer-overflow"))) {\
	return (X);\
    }())
#else
# define UNSIGNED_OVERFLOW_OK(X) (X)
#endif

/* Signal we're building the library so it's OK to include headers such as
 * xapian/query.h directly.
 */
#define XAPIAN_LIB_BUILD 1
