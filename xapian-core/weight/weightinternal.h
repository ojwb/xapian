/** @file weightinternal.h
 * @brief Xapian::Weight::Internal class, holding database and term statistics.
 */
/* Copyright (C) 2007 Lemur Consulting Ltd
 * Copyright (C) 2009,2010,2011,2013,2014,2015 Olly Betts
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#ifndef XAPIAN_INCLUDED_WEIGHTINTERNAL_H
#define XAPIAN_INCLUDED_WEIGHTINTERNAL_H

#include "xapian/weight.h"

#include "xapian/database.h"
#include "xapian/query.h"

#include "backends/database.h"
#include "internaltypes.h"
#include "omassert.h"

#include <map>
#include <string>

/// The frequencies for a term.
struct TermFreqs {
    Xapian::doccount termfreq;
    Xapian::doccount reltermfreq;
    Xapian::termcount collfreq;
    double max_part;

    TermFreqs() : termfreq(0), reltermfreq(0), collfreq(0), max_part(0.0) {}
    TermFreqs(Xapian::doccount termfreq_,
	      Xapian::doccount reltermfreq_,
	      Xapian::termcount collfreq_,
	      double max_part_ = 0.0)
	: termfreq(termfreq_),
	  reltermfreq(reltermfreq_),
	  collfreq(collfreq_),
	  max_part(max_part_) {}

    void operator +=(const TermFreqs & other) {
	termfreq += other.termfreq;
	reltermfreq += other.reltermfreq;
	collfreq += other.collfreq;
	max_part += other.max_part;
    }

    /// Return a std::string describing this object.
    std::string get_description() const;
};

namespace Xapian {

class RSet;

/** Class to hold statistics for a given collection. */
struct Weight::Internal {
    unsigned collection_size;
    unsigned rset_size;
    unsigned total_term_count;
    unsigned total_length;
    bool have_max_part;
    std::string termfreqs;
    double get_average_length() const {
	return collection_size == 0 ? 0.0 : double(total_length) / collection_size;
    }
    bool get_stats(const std::string&, Xapian::doccount& tf, Xapian::doccount& reltf, Xapian::termcount& cf) const {
	tf = reltf = cf = 0;
	return true;
    }
    bool get_termweight(const std::string&, double& relevance) const {
	relevance = 0.0;
	return true;
    }
};

}

#endif // XAPIAN_INCLUDED_WEIGHTINTERNAL_H
