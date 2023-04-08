/** @file
 * @brief Negate unsigned integer, avoiding compiler warnings
 */
/* Copyright (C) 2023 Olly Betts
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#ifndef XAPIAN_INCLUDED_NEGATE_UNSIGNED_H
#define XAPIAN_INCLUDED_NEGATE_UNSIGNED_H

#include <type_traits>

template<typename T>
constexpr typename std::enable_if<std::is_unsigned<T>::value, T>::type
negate_unsigned(T value)
{
#ifdef _MSC_VER
// Suppress warning about negating an unsigned type, which we do deliberately
// in a few places in the code.
# pragma warning(push)
# pragma warning(disable:4146)
#endif
    return -value;
#ifdef _MSC_VER
# pragma warning(pop)
#endif
}

#endif // XAPIAN_INCLUDED_NEGATE_UNSIGNED_H
