/** @file postingsource.cc
 * @brief External sources of posting information
 */
/* Copyright (C) 2008,2009,2010,2011,2012,2015,2016 Olly Betts
 * Copyright (C) 2008,2009 Lemur Consulting Ltd
 * Copyright (C) 2010 Richard Boulton
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

#include <config.h>

// We need to be able to set deprecated members of ValuePostingSource.
#define XAPIAN_DEPRECATED(X) X
#include "xapian/postingsource.h"

#include "autoptr.h"

#include "backends/database.h"
#include "backends/document.h"
#include "matcher/multimatch.h"

#include "xapian/document.h"
#include "xapian/error.h"
#include "xapian/queryparser.h" // For sortable_unserialise().

#include "omassert.h"
#include "net/length.h"
#include "serialise-double.h"
#include "str.h"

#include <cfloat>

using namespace std;

namespace Xapian {

PostingSource::~PostingSource() { }

void
PostingSource::set_maxweight(double max_weight)
{
#if 0
    if (usual(matcher_)) {
	MultiMatch * multimatch = static_cast<MultiMatch*>(matcher_);
	multimatch->recalc_maxweight();
    }
#endif
    max_weight_ = max_weight;
}

double
PostingSource::get_weight() const
{
    return 0;
}

void
PostingSource::skip_to(Xapian::docid did, double min_wt)
{
    while (!at_end() && get_docid() < did) {
	next(min_wt);
    }
}

bool
PostingSource::check(Xapian::docid did, double min_wt)
{
    skip_to(did, min_wt);
    return true;
}

PostingSource *
PostingSource::clone() const
{
    return NULL;
}

string
PostingSource::name() const
{
    return string();
}

string
PostingSource::serialise() const
{
    throw Xapian::UnimplementedError("serialise() not supported for this PostingSource");
}

PostingSource *
PostingSource::unserialise(const string &) const
{
    throw Xapian::UnimplementedError("unserialise() not supported for this PostingSource");
}

PostingSource *
PostingSource::unserialise_with_registry(const std::string &s,
					 const Registry &) const
{
    return unserialise(s);
}

string
PostingSource::get_description() const
{
    return "Xapian::PostingSource subclass";
}

}
