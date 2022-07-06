/** @file
 * @brief N-way OR postlist with wt=max(wt_i)
 */
/* Copyright (C) 2007,2009,2010,2011,2012,2013,2017,2018 Olly Betts
 * Copyright (C) 2009 Lemur Consulting Ltd
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

#ifndef XAPIAN_INCLUDED_MAXPOSTLIST_H
#define XAPIAN_INCLUDED_MAXPOSTLIST_H

#include "backends/postlist.h"
#include "postlisttree.h"

#include <algorithm>

/// N-way OR postlist with wt=max(wt_i).
class MaxPostList : public PostList {
    /// Don't allow assignment.
    void operator=(const MaxPostList &);

    /// Don't allow copying.
    MaxPostList(const MaxPostList &);

    /// The current docid, or zero if we haven't started or are at_end.
    Xapian::docid did = 0;

    /// The number of sub-postlists.
    size_t n_kids;

    /// Array of pointers to sub-postlists.
    PostList** plist = nullptr;

    /// Pointer to the matcher object, so we can report pruning.
    PostListTree *matcher;

    /// Erase a sub-postlist.
    void erase_sublist(size_t i) {
	delete plist[i];
	--n_kids;
	for (size_t j = i; j < n_kids; ++j) {
	    plist[j] = plist[j + 1];
	}
	matcher->force_recalc();
    }

  public:
    /** Construct from 2 random-access iterators to a container of PostList*,
     *  a pointer to the matcher, and the document collection size.
     */
    template<class RandomItor>
    MaxPostList(RandomItor pl_begin, RandomItor pl_end,
		PostListTree* matcher_, Xapian::doccount db_size)
	: n_kids(pl_end - pl_begin), matcher(matcher_)
    {
	plist = new PostList * [n_kids];
	std::copy(pl_begin, pl_end, plist);

	// We shortcut an empty shard and avoid creating a postlist tree for it.
	Assert(db_size);

	// We calculate the estimate assuming independence.  The simplest
	// way to calculate this seems to be a series of (n_kids - 1) pairwise
	// calculations, which gives the same answer regardless of the order.
	double scale = 1.0 / db_size;
	double P_est = plist[0]->get_termfreq() * scale;
	for (size_t i = 1; i < n_kids; ++i) {
	    double P_i = plist[i]->get_termfreq() * scale;
	    P_est += P_i - P_est * P_i;
	}
	termfreq = static_cast<Xapian::doccount>(P_est * db_size + 0.5);
    }

    ~MaxPostList();

    Xapian::docid get_docid() const;

    double get_weight(Xapian::termcount doclen,
		      Xapian::termcount unique_terms,
		      Xapian::termcount wdfdocmax) const;

    bool at_end() const;

    double recalc_maxweight();

    PositionList * read_position_list() {
	return NULL;
    }

    PostList* next(double w_min);

    PostList* skip_to(Xapian::docid, double w_min);

    void get_docid_range(Xapian::docid& first, Xapian::docid& last) const;

    std::string get_description() const;

    /** get_wdf() for MaxPostlist returns the sum of the wdfs of the
     *  sub postlists which match the current docid.
     *
     *  The wdf isn't really meaningful in many situations, but if the lists
     *  are being combined as a synonym we want the sum of the wdfs, so we do
     *  that in general.
     */
    Xapian::termcount get_wdf() const;

    Xapian::termcount count_matching_subqs() const;
};

#endif // XAPIAN_INCLUDED_MAXPOSTLIST_H
