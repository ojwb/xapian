/* confdefs.h */
#define PACKAGE_NAME "xapian-core"
#define PACKAGE_TARNAME "xapian-core"
#define PACKAGE_VERSION "1.5.0"
#define PACKAGE_STRING "xapian-core 1.5.0"
#define PACKAGE_BUGREPORT "https://xapian.org/bugs"
#define PACKAGE_URL ""
#define PACKAGE "xapian-core"
#define VERSION "1.5.0"
#define HAVE_STDIO_H 1
#define HAVE_STDLIB_H 1
#define HAVE_STRING_H 1
#define HAVE_INTTYPES_H 1
#define HAVE_STDINT_H 1
#define HAVE_STRINGS_H 1
#define HAVE_SYS_STAT_H 1
#define HAVE_SYS_TYPES_H 1
#define HAVE_UNISTD_H 1
#define STDC_HEADERS 1
#define LT_OBJDIR ".libs/"
#define HAVE_CXX17 1
#define _FILE_OFFSET_BITS 64
#define HAVE_DECL_EXP10 0
#define HAVE_DECL___EXP10 0
#define USE_RTTI 1
#define HAVE_DECL___BUILTIN_ADD_OVERFLOW 1
#define HAVE_DECL___BUILTIN_SUB_OVERFLOW 1
#define HAVE_DECL___BUILTIN_MUL_OVERFLOW 1
#define HAVE_DECL___BUILTIN_BSWAP16 1
#define HAVE_DECL___BUILTIN_BSWAP32 1
#define HAVE_DECL___BUILTIN_BSWAP64 1
#define HAVE_DECL__BYTESWAP_USHORT 1
#define HAVE_DECL__BYTESWAP_ULONG 1
#define HAVE_DECL__BYTESWAP_UINT64 1
#define HAVE_DECL___BUILTIN_CLZ 1
#define HAVE_DECL___BUILTIN_CLZL 1
#define HAVE_DECL___BUILTIN_CLZLL 1
#define HAVE_DECL___BUILTIN_CTZ 1
#define HAVE_DECL___BUILTIN_CTZL 1
#define HAVE_DECL___BUILTIN_CTZLL 1
#define HAVE_DECL___BUILTIN_EXPECT 1
#define HAVE_DECL___BUILTIN_POPCOUNT 1
#define HAVE_DECL___BUILTIN_POPCOUNTL 1
#define HAVE_DECL___BUILTIN_POPCOUNTLL 1
#define HAVE_DECL___POPCNT 1
#define HAVE_DECL___POPCNT64 1
#define HAVE_DECL__ADDCARRY_U32 1
#define HAVE_DECL__ADDCARRY_U64 1
#define HAVE_DECL__SUBBORROW_U32 1
#define HAVE_DECL__SUBBORROW_U64 1
#define HAVE_CLOCK_GETTIME 1
#define HAVE_SLEEP 1
#define HAVE_NANOSLEEP 1
#define HAVE_GETTIMEOFDAY 1
#define HAVE_FTIME 1
#define __MSVCRT_VERSION__ 0x0601
#define MINGW_HAS_SECURE_API 1
#define HAVE_DECL_SIGSETJMP 0
#define HAVE_DECL_SIGLONGJMP 0
#define HAVE_NFTW 1
#define HAVE__PUTENV_S 1
#define HAVE_DECL__PUTENV_S 0
#define FTIME_RETURNS_VOID 1
#define ssize_t int
#define pid_t __int64
#define mode_t int
/* end confdefs.h.  */
#include <stddef.h>
#ifdef HAVE_STDIO_H
# include <stdio.h>
#endif
#ifdef HAVE_STDLIB_H
# include <stdlib.h>
#endif
#ifdef HAVE_STRING_H
# include <string.h>
#endif
#ifdef HAVE_INTTYPES_H
# include <inttypes.h>
#endif
#ifdef HAVE_STDINT_H
# include <stdint.h>
#endif
#ifdef HAVE_STRINGS_H
# include <strings.h>
#endif
#ifdef HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif
#ifdef HAVE_SYS_STAT_H
# include <sys/stat.h>
#endif
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif
static long int longval (void) { return (long int) (sizeof (short)); }
static unsigned long int ulongval (void) { return (long int) (sizeof (short)); }
#include <stdio.h>
#include <stdlib.h>
int
main (void)
{

  FILE *f = fopen ("conftest.val", "w");
  if (! f)
    return 1;
  if (((long int) (sizeof (short))) < 0)
    {
      long int i = longval ();
      if (i != ((long int) (sizeof (short))))
	return 1;
      fprintf (f, "%ld", i);
    }
  else
    {
      unsigned long int i = ulongval ();
      if (i != ((long int) (sizeof (short))))
	return 1;
      fprintf (f, "%lu", i);
    }
  /* Do not output a trailing newline, as this causes \r\n confusion
     on some platforms.  */
  return ferror (f) || fclose (f) != 0;

  ;
  return 0;
}
