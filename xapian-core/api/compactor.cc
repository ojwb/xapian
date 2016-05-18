/** @file compactor.cc
 * @brief Compact a database, or merge and compact several.
 */
/* Copyright (C) 2003,2004,2005,2006,2007,2008,2009,2010,2011,2012,2013,2015,2016 Olly Betts
 * Copyright (C) 2008 Lemur Consulting Ltd
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
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301
 * USA
 */

#include <config.h>

#include <xapian/compactor.h>

#include "safeerrno.h"

#include <algorithm>
#include <fstream>
#include <vector>

#include <cstring>
#include <ctime>
#include "safesysstat.h"
#include <sys/types.h>

#include "safeunistd.h"
#include "safefcntl.h"

#include "backends/backends.h"
#include "backends/database.h"
#include "debuglog.h"
#include "leafpostlist.h"
#include "noreturn.h"
#include "omassert.h"
#include "filetests.h"
#include "fileutils.h"
#include "io_utils.h"
#include "stringutils.h"
#include "str.h"

#ifdef XAPIAN_HAS_GLASS_BACKEND
#include "backends/glass/glass_database.h"
#include "backends/glass/glass_version.h"
#endif
#ifdef XAPIAN_HAS_CHERT_BACKEND
#include "backends/chert/chert_database.h"
#include "backends/chert/chert_version.h"
#endif

#include <xapian/constants.h>
#include <xapian/database.h>
#include <xapian/error.h>

using namespace std;

class CmpByFirstUsed {
    const vector<pair<Xapian::docid, Xapian::docid> > & used_ranges;

  public:
    CmpByFirstUsed(const vector<pair<Xapian::docid, Xapian::docid> > & ur)
	: used_ranges(ur) { }

    bool operator()(size_t a, size_t b) {
	return used_ranges[a].first < used_ranges[b].first;
    }
};

namespace Xapian {

class Compactor::Internal : public Xapian::Internal::intrusive_base {
    friend class Compactor;

    string destdir_compat;
    size_t block_size;
    unsigned flags;

    vector<string> srcdirs_compat;

  public:
    Internal() : block_size(8192), flags(FULL) { }
};

Compactor::Compactor() : internal(new Compactor::Internal()) { }

Compactor::~Compactor() { }

void
Compactor::set_block_size(size_t block_size)
{
    internal->block_size = block_size;
}

void
Compactor::set_flags_(unsigned flags, unsigned mask)
{
    internal->flags = (internal->flags & mask) | flags;
}

void
Compactor::set_destdir(const string & destdir)
{
    internal->destdir_compat = destdir;
}

void
Compactor::add_source(const string & srcdir)
{
    internal->srcdirs_compat.push_back(srcdir);
}

void
Compactor::set_status(const string & table, const string & status)
{
    (void)table;
    (void)status;
}

string
Compactor::resolve_duplicate_metadata(const string & key,
				      size_t num_tags, const std::string tags[])
{
    (void)key;
    (void)num_tags;
    return tags[0];
}

}

