/** @file
 *  @brief Calculated bounds on and estimate of number of matches
 */
/* Copyright (C) 2022 Olly Betts
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

#ifndef XAPIAN_INCLUDED_ESTIMATEOP_H
#define XAPIAN_INCLUDED_ESTIMATEOP_H

#include "xapian/types.h"

struct Estimates {
    Xapian::doccount min, est, max;

    /** Lower bound on docids matched. */
    Xapian::docid first;

    /** Upper bound on docids matched. */
    Xapian::docid last;

    Estimates() { }

    Estimates(Xapian::doccount min_,
	      Xapian::doccount est_,
	      Xapian::doccount max_,
	      Xapian::doccount first_ = 1,
	      Xapian::doccount last_ = Xapian::docid(-1))
	: min(min_), est(est_), max(max_), first(first_), last(last_) { }
};

// Clean up Microsoft namespace pollution.
#ifdef NEAR
# undef NEAR
#endif

class EstimateOp {
  public:
    enum op_type {
	KNOWN,
	// In the absence of accept/reject counts we just scale the AND by
	// dividing by these values:
	DECIDER = 1,
	NEAR = 2, PHRASE = 3, EXACT_PHRASE = 4,
	POSTING_SOURCE,
	AND, AND_NOT, OR, XOR
    };

  private:
    EstimateOp* next;

    op_type type;

    /** Estimates.
     *
     *  * KNOWN: Already known exact leaf term frequency (in min/est/max)
     *    or value range min/est/max.
     *  * DECIDER/NEAR/PHRASE/EXACT_PHRASE: Filled in on PostList deletion by
     *    calling report_ratio(): min = accepted, max = rejected
     *  * POSTING_SOURCE: Filled in on PostList construction with min/est/max
     *    from PostingSource object.
     */
    Estimates estimates;

    /** Used by get_subquery_count().
     *
     *  Set to zero for leaf operators.
     */
    unsigned n_subqueries = 0;

  public:
    /// Leaf term.
    EstimateOp(EstimateOp* next_, Xapian::doccount tf_,
	       Xapian::docid first, Xapian::docid last)
	: next(next_), type(KNOWN), estimates(tf_, tf_, tf_, first, last) { }

    /// PostingSource
    EstimateOp(EstimateOp* next_)
	: next(next_), type(POSTING_SOURCE) { }

    /// Value range.
    EstimateOp(EstimateOp* next_, Estimates estimates_)
	: next(next_), type(KNOWN), estimates(estimates_) { }

    /// Value range degenerate case.
    EstimateOp(EstimateOp* next_, Xapian::doccount tf_)
	: next(next_), type(KNOWN), estimates(tf_, tf_, tf_) { }

    /// AND, AND_NOT, OR or XOR.
    EstimateOp(EstimateOp* next_, op_type type_, unsigned n_subqueries_,
	       Xapian::docid first, Xapian::docid last)
	: next(next_), type(type_), n_subqueries(n_subqueries_) {
	estimates.first = first;
	estimates.last = last;
    }

    /** NEAR, PHRASE or EXACT_PHRASE.
     *
     *  These operate as filters so have a single subquery.
     */
    EstimateOp(EstimateOp* next_, op_type type_)
	: next(next_), type(type_), estimates(0, 0, 0), n_subqueries(1) { }

    void report_ratio(Xapian::doccount accepted, Xapian::doccount rejected) {
	estimates.min = accepted;
	estimates.max = rejected;
    }

    /** Fill in estimates for POSTING_SOURCE. */
    void report_termfreqs(Xapian::doccount min_,
			  Xapian::doccount est,
			  Xapian::doccount max_) {
	estimates.min = min_;
	estimates.est = est;
	estimates.max = max_;
	estimates.first = 1;
	estimates.last = Xapian::docid(-1);
    }

    Estimates resolve(Xapian::doccount db_size,
		      Xapian::docid db_first,
		      Xapian::docid db_last);

    Estimates resolve_next(Xapian::doccount db_size,
			   Xapian::docid db_first,
			   Xapian::docid db_last) {
	Estimates result = next->resolve(db_size, db_first, db_last);
	EstimateOp* old_next = next;
	next = next->next;
	delete old_next;
	return result;
    }

    EstimateOp* get_next() const { return next; }

    unsigned get_subquery_count() const { return n_subqueries; }
};

#endif // XAPIAN_INCLUDED_ESTIMATEOP_H
