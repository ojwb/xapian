/** @file glass_version.cc
 * @brief GlassVersion class
 */
/* Copyright (C) 2006,2007,2008,2009,2010,2013,2014,2015 Olly Betts
 * Copyright (C) 2011 Dan Colish
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <config.h>

#include "glass_version.h"

#include "debuglog.h"
#include "fd.h"
#include "io_utils.h"
#include "omassert.h"
#include "pack.h"
#include "posixy_wrapper.h"
#include "stringutils.h" // For STRINGIZE() and CONST_STRLEN().

#include <cstring> // For memcmp().
#include <string>
#include "safeerrno.h"
#include <sys/types.h>
#include "safesysstat.h"
#include "safefcntl.h"
#include "safeunistd.h"
#include "str.h"
#include "stringutils.h"

#include "common/safeuuid.h"

#include "xapian/constants.h"
#include "xapian/error.h"

using namespace std;

/// Glass format version (date of change):
#define GLASS_FORMAT_VERSION DATE_TO_VERSION(2015,12,01)
// 2015,12,01 1.3.4 2 bytes "components_of" per item eliminated; more
// 2014,11,21 1.3.2 Brass renamed to Glass

/// Convert date <-> version number.  Dates up to 2141-12-31 fit in 2 bytes.
#define DATE_TO_VERSION(Y,M,D) \
	((unsigned(Y) - 2014) << 9 | unsigned(M) << 5 | unsigned(D))
#define VERSION_TO_YEAR(V) ((unsigned(V) >> 9) + 2014)
#define VERSION_TO_MONTH(V) ((unsigned(V) >> 5) & 0x0f)
#define VERSION_TO_DAY(V) (unsigned(V) & 0x1f)

#define GLASS_VERSION_MAGIC_LEN 14
#define GLASS_VERSION_MAGIC_AND_VERSION_LEN 16

static const char GLASS_VERSION_MAGIC[GLASS_VERSION_MAGIC_AND_VERSION_LEN] = {
    '\x0f', '\x0d', 'X', 'a', 'p', 'i', 'a', 'n', ' ', 'G', 'l', 'a', 's', 's',
    char((GLASS_FORMAT_VERSION >> 8) & 0xff), char(GLASS_FORMAT_VERSION & 0xff)
};

GlassVersion::~GlassVersion()
{
    // Either this is a single-file database, or this fd is from opening a new
    // version file in write(), but sync() was never called.
    if (fd != -1)
	(void)::close(fd);
}

void
GlassVersion::set_fd(int fd_)
{
    db_dir = string();
    fd = fd_;
    offset = lseek(fd, 0, SEEK_CUR);
    if (rare(offset == off_t(-1))) {
	string msg = "lseek failed on file descriptor ";
	msg += str(fd);
	throw Xapian::DatabaseOpeningError(msg, errno);
    }
}

void
GlassVersion::read()
{
    LOGCALL_VOID(DB, "GlassVersion::read", NO_ARGS);
    FD close_fd(-1);
    int fd_in;
    if (single_file()) {
	if (rare(lseek(fd, offset, SEEK_SET) == off_t(-1))) {
	    string msg = "Failed to rewind file descriptor ";
	    msg += str(fd);
	    throw Xapian::DatabaseOpeningError(msg, errno);
	}
	fd_in = fd;
    } else {
	string filename = db_dir;
	filename += "/iamglass";
	fd_in = posixy_open(filename.c_str(), O_RDONLY|O_BINARY);
	if (rare(fd_in < 0)) {
	    string msg = filename;
	    msg += ": Failed to open glass revision file for reading";
	    throw Xapian::DatabaseOpeningError(msg, errno);
	}
	close_fd = fd_in;
    }

    char buf[256];

    const char * p = buf;
    const char * end = p + io_read(fd_in, buf, sizeof(buf), 33);

    if (memcmp(buf, GLASS_VERSION_MAGIC, GLASS_VERSION_MAGIC_LEN) != 0)
	throw Xapian::DatabaseCorruptError("Rev file magic incorrect");

    unsigned version;
    version = static_cast<unsigned char>(buf[GLASS_VERSION_MAGIC_LEN]);
    version <<= 8;
    version |= static_cast<unsigned char>(buf[GLASS_VERSION_MAGIC_LEN + 1]);
    if (version != GLASS_FORMAT_VERSION) {
	string msg;
	if (!single_file()) {
	    msg = db_dir;
	    msg += ": ";
	}
	msg += "Database is format version ";
	msg += str(VERSION_TO_YEAR(version) * 10000 +
		   VERSION_TO_MONTH(version) * 100 +
		   VERSION_TO_DAY(version));
	msg += " but I only understand ";
	msg += str(VERSION_TO_YEAR(GLASS_FORMAT_VERSION) * 10000 +
		   VERSION_TO_MONTH(GLASS_FORMAT_VERSION) * 100 +
		   VERSION_TO_DAY(GLASS_FORMAT_VERSION));
	throw Xapian::DatabaseVersionError(msg);
    }

    p += GLASS_VERSION_MAGIC_AND_VERSION_LEN;
    memcpy((void*)uuid, p, 16);
    p += 16;

    if (!unpack_uint(&p, end, &rev))
	throw Xapian::DatabaseCorruptError("Rev file failed to decode revision");

    for (unsigned table_no = 0; table_no < Glass::MAX_; ++table_no) {
	if (!root[table_no].unserialise(&p, end)) {
	    throw Xapian::DatabaseCorruptError("Rev file root_info missing");
	}
	old_root[table_no] = root[table_no];
    }

    // For a single-file database, this will assign extra data.  We read
    // sizeof(buf) above, then skip GLASS_VERSION_MAGIC_AND_VERSION_LEN,
    // then 16, then the size of the serialised root info.
    serialised_stats.assign(p, end);
    unserialise_stats();
}

void
GlassVersion::serialise_stats()
{
    serialised_stats.resize(0);
    pack_uint(serialised_stats, doccount);
    // last_docid must always be >= doccount.
    pack_uint(serialised_stats, last_docid - doccount);
    pack_uint(serialised_stats, doclen_lbound);
    pack_uint(serialised_stats, wdf_ubound);
    // doclen_ubound should always be >= wdf_ubound, so we store the
    // difference as it may encode smaller.  wdf_ubound is likely to
    // be larger than doclen_lbound.
    pack_uint(serialised_stats, doclen_ubound - wdf_ubound);
    pack_uint(serialised_stats, oldest_changeset);
    pack_uint(serialised_stats, total_doclen);
}

void
GlassVersion::unserialise_stats()
{
    const char * p = serialised_stats.data();
    const char * end = p + serialised_stats.size();
    if (p == end) {
	doccount = 0;
	total_doclen = 0;
	last_docid = 0;
	doclen_lbound = 0;
	doclen_ubound = 0;
	wdf_ubound = 0;
	oldest_changeset = 0;
	return;
    }

    if (!unpack_uint(&p, end, &doccount) ||
	!unpack_uint(&p, end, &last_docid) ||
	!unpack_uint(&p, end, &doclen_lbound) ||
	!unpack_uint(&p, end, &wdf_ubound) ||
	!unpack_uint(&p, end, &doclen_ubound) ||
	!unpack_uint(&p, end, &oldest_changeset) ||
	!unpack_uint(&p, end, &total_doclen)) {
	const char * m = p ?
	    "Bad serialised DB stats (overflowed)" :
	    "Bad serialised DB stats (out of data)";
	throw Xapian::DatabaseCorruptError(m);
    }

    // In the single-file DB case, there will be extra data in
    // serialised_stats, so suppress this check.
    if (p != end && !single_file())
	throw Xapian::DatabaseCorruptError("Rev file has junk at end");

    // last_docid must always be >= doccount.
    last_docid += doccount;
    // doclen_ubound should always be >= wdf_ubound, so we store the
    // difference as it may encode smaller.  wdf_ubound is likely to
    // be larger than doclen_lbound.
    doclen_ubound += wdf_ubound;
}

void
GlassVersion::merge_stats(const GlassVersion & o)
{
    doccount += o.get_doccount();
    if (doccount < o.get_doccount()) {
	throw "doccount wrapped!";
    }

    Xapian::termcount o_doclen_lbound = o.get_doclength_lower_bound();
    if (o_doclen_lbound > 0) {
	if (doclen_lbound == 0 || o_doclen_lbound < doclen_lbound)
	    doclen_lbound = o_doclen_lbound;
    }

    doclen_ubound = max(doclen_ubound, o.get_doclength_upper_bound());
    wdf_ubound = max(wdf_ubound, o.get_wdf_upper_bound());
    total_doclen += o.get_total_doclen();
    if (total_doclen < o.get_total_doclen()) {
	throw "totlen wrapped!";
    }
}

void
GlassVersion::cancel()
{
    LOGCALL_VOID(DB, "GlassVersion::cancel", NO_ARGS);
    for (unsigned table_no = 0; table_no < Glass::MAX_; ++table_no) {
	root[table_no] = old_root[table_no];
    }
    unserialise_stats();
}

const string
GlassVersion::write(glass_revision_number_t new_rev, int flags)
{
    LOGCALL(DB, const string, "GlassVersion::write", new_rev|flags);

    string s(GLASS_VERSION_MAGIC, GLASS_VERSION_MAGIC_AND_VERSION_LEN);
    s.append((const char *)uuid, 16);

    pack_uint(s, new_rev);

    for (unsigned table_no = 0; table_no < Glass::MAX_; ++table_no) {
	root[table_no].serialise(s);
    }

    // Serialise database statistics.
    serialise_stats();
    s += serialised_stats;

    string tmpfile;
    if (!single_file()) {
	tmpfile = db_dir;
	// In dangerous mode, just write the new version file in place.
	if (flags & Xapian::DB_DANGEROUS)
	    tmpfile += "/iamglass";
	else
	    tmpfile += "/v.tmp";

	fd = posixy_open(tmpfile.c_str(), O_CREAT|O_TRUNC|O_WRONLY|O_BINARY, 0666);
	if (rare(fd < 0))
	    throw Xapian::DatabaseOpeningError("Couldn't write new rev file: " + tmpfile,
					       errno);

	if (flags & Xapian::DB_DANGEROUS)
	    tmpfile = string();
    }

    try {
	io_write(fd, s.data(), s.size());
    } catch (...) {
	if (!single_file())
	    (void)close(fd);
	throw;
    }

    if (changes) {
	string changes_buf;
	changes_buf += '\xfe';
	pack_uint(changes_buf, new_rev);
	pack_uint(changes_buf, s.size());
	changes->write_block(changes_buf);
	changes->write_block(s);
    }

    RETURN(tmpfile);
}

bool
GlassVersion::sync(const string & tmpfile,
		   glass_revision_number_t new_rev, int flags)
{
    Assert(new_rev > rev || rev == 0);

    if (single_file()) {
	if ((flags & Xapian::DB_NO_SYNC) == 0 &&
	    ((flags & Xapian::DB_FULL_SYNC) ?
	      !io_full_sync(fd) :
	      !io_sync(fd))) {
	    // FIXME what to do?
	}
    } else {
	int fd_to_close = fd;
	fd = -1;
	if ((flags & Xapian::DB_NO_SYNC) == 0 &&
	    ((flags & Xapian::DB_FULL_SYNC) ?
	      !io_full_sync(fd_to_close) :
	      !io_sync(fd_to_close))) {
	    int save_errno = errno;
	    (void)close(fd_to_close);
	    if (!tmpfile.empty())
		(void)unlink(tmpfile.c_str());
	    errno = save_errno;
	    return false;
	}

	if (close(fd_to_close) != 0) {
	    if (!tmpfile.empty()) {
		int save_errno = errno;
		(void)unlink(tmpfile.c_str());
		errno = save_errno;
	    }
	    return false;
	}

	if (!tmpfile.empty()) {
	    string filename = db_dir;
	    filename += "/iamglass";

	    if (posixy_rename(tmpfile.c_str(), filename.c_str()) < 0) {
		// Over NFS, rename() can sometimes report failure when the
		// operation succeeded, so in this case we try to unlink the source
		// to check if the rename really failed.
		int save_errno = errno;
		if (unlink(tmpfile.c_str()) == 0 || errno != ENOENT) {
		    errno = save_errno;
		    return false;
		}
	    }
	}
    }

    for (unsigned table_no = 0; table_no < Glass::MAX_; ++table_no) {
	old_root[table_no] = root[table_no];
    }

    rev = new_rev;
    return true;
}

void
GlassVersion::create(unsigned blocksize, int flags)
{
    AssertRel(blocksize,>=,2048);
    uuid_generate(uuid);
    for (unsigned table_no = 0; table_no < Glass::MAX_; ++table_no) {
	root[table_no].init(blocksize);
    }
    sync(write(rev, flags), rev, flags);
}

namespace Glass {

void
RootInfo::init(unsigned blocksize_)
{
    AssertRel(blocksize_,>=,2048);
    root = 0;
    level = 0;
    num_entries = 0;
    root_is_fake = true;
    sequential_mode = true;
    blocksize = blocksize_;
    fl_serialised.resize(0);
}

void
RootInfo::serialise(string &s) const
{
    pack_uint(s, root);
    unsigned val = level << 2;
    if (sequential_mode) val |= 0x02;
    if (root_is_fake) val |= 0x01;
    pack_uint(s, val);
    pack_uint(s, num_entries);
    pack_uint(s, blocksize >> 11);
    pack_string(s, fl_serialised);
}

bool
RootInfo::unserialise(const char ** p, const char * end)
{
    unsigned val;
    if (!unpack_uint(p, end, &root) ||
	!unpack_uint(p, end, &val) ||
	!unpack_uint(p, end, &num_entries) ||
	!unpack_uint(p, end, &blocksize) ||
	!unpack_string(p, end, fl_serialised)) return false;
    level = val >> 2;
    sequential_mode = val & 0x02;
    root_is_fake = val & 0x01;
    blocksize <<= 11;
    AssertRel(blocksize,>=,2048);
    return true;
}

}
