/** @file
 * @brief <unistd.h>, but with compat. and large file support for MSVC.
 */
/* Copyright (C) 2007,2015,2026 Olly Betts
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see
 * <https://www.gnu.org/licenses/>.
 */

#ifndef XAPIAN_INCLUDED_SAFEUNISTD_H
#define XAPIAN_INCLUDED_SAFEUNISTD_H

#ifndef PACKAGE
# error config.h must be included first in each C++ source file
#endif

#ifndef _MSC_VER
# include <unistd.h>
#else

// io.h is the nearest equivalent to unistd.h.
# include <io.h>

// process.h is needed for getpid().
# include <process.h>

// direct.h is needed for rmdir().
# include <direct.h>

#endif

#ifdef __WIN32__

# include <climits>

// Under mingw we probably don't need to provide our own sleep().
# ifndef HAVE_SLEEP

inline unsigned int
sleep(unsigned int seconds)
{
    // Use our own little helper function to avoid pulling in <windows.h>.
    extern void xapian_sleep_milliseconds(unsigned int millisecs);

    // Sleep takes a time interval in milliseconds, whereas POSIX sleep takes
    // a time interval in seconds, so we need to multiply 'seconds' by 1000.
    //
    // But make sure the multiplication won't overflow!  4294967 seconds is
    // nearly 50 days, so just sleep for that long and return the number of
    // seconds left to sleep for.  The common case of sleep(CONSTANT) should
    // optimise to just xapian_sleep_milliseconds(CONSTANT).
    if (seconds > 4294967u) {
        xapian_sleep_milliseconds(4294967000u);
        return seconds - 4294967u;
    }
    xapian_sleep_milliseconds(seconds * 1000u);
    return 0;
}

# endif

// POSIX `read()` takes `size_t count`, but Microsoft's takes `unsigned`.
// This creates potential for bugs because large values will be truncated
// to fit the type so a read of 4GB (or an exact multiple thereof) will
// truncate to 0 and `read()` will return 0 incorrectly indicating EOF.
//
// Also, `count > INT_MAX` is treated as invalid.
//
// We provide a templated replacement which returns a short read if `count >
// INT_MAX`.
//
// POSIX `read()` returns `ssize_t` ratehr than `int`, but we don't currently
// attempt to emulate that since that can't result in truncation.
template<typename S>
inline int
read(int fd, void* buf, S count)
{
    unsigned c =
        rare(count > S{INT_MAX}) ? unsigned{INT_MAX} : unsigned(count);
    return _read(fd, buf, c);
}

// Similarly for `write()`.
template<typename S>
inline int
write(int fd, const void* buf, S count)
{
    unsigned c =
        rare(count > S{INT_MAX}) ? unsigned{INT_MAX} : unsigned(count);
    return _write(fd, buf, c);
}

#endif

#endif /* XAPIAN_INCLUDED_SAFEUNISTD_H */
