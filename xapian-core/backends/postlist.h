/** @file
 * @brief Abstract base class for postlists.
 */
/* Copyright (C) 2007-2022 Olly Betts
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

#ifndef XAPIAN_INCLUDED_POSTLIST_H
#define XAPIAN_INCLUDED_POSTLIST_H

#include <string>

#include "xapian/intrusive_ptr.h"
#include <xapian/types.h>
#include <xapian/postingiterator.h>

#include "backends/positionlist.h"
#include "weight/weightinternal.h"

class OrPositionList;

namespace Xapian {
namespace Internal {

/// Abstract base class for postlists.
class PostList {
    /// Don't allow assignment.
    void operator=(const PostList &) = delete;

    /// Don't allow copying.
    PostList(const PostList &) = delete;

  protected:
    /** Estimate of the number of documents this PostList will return.
     *
     *  This should be exact for terms.
     */
    Xapian::doccount termfreq;

    /// Only constructable as a base class for derived classes.
    PostList() { }

  public:
    /** We have virtual methods and want to be able to delete derived classes
     *  using a pointer to the base class, so we need a virtual destructor.
     */
    virtual ~PostList();

    /** Get an estimate of the number of documents this PostList will return.
     *
     *  This should be exact for terms.
     */
    Xapian::doccount get_termfreq() const { return termfreq; }

    /// Return the current docid.
    virtual Xapian::docid get_docid() const = 0;

    /** Return the wdf for the document at the current position.
     *
     *  The default implementation throws Xapian::UnimplementedError.
     */
    virtual Xapian::termcount get_wdf() const;

    /// Return the weight contribution for the current position.
    virtual double get_weight(Xapian::termcount doclen,
			      Xapian::termcount unique_terms,
			      Xapian::termcount wdfdocmax) const = 0;

    /// Return true if the current position is past the last entry in this list.
    virtual bool at_end() const = 0;

    /** Recalculate the upper bound on what get_weight() can return.
     *
     *  The maximum weight that get_weight() can return can decrease as the
     *  match progresses (typically when the PostList tree prunes) - calling
     *  this method calculates a current upper bound.
     *
     *  Note that this method may be called after the postlist has reached the
     *  end.  In this situation, the method should return 0.
     */
    virtual double recalc_maxweight() = 0;

    /** Read the position list for the term in the current document and
     *  return a pointer to it (owned by the PostList).
     *
     *  The default implementation throws Xapian::UnimplementedError.
     */
    virtual PositionList * read_position_list();

    /** Read the position list for the term in the current document and
     *  return a pointer to it (not owned by the PostList).
     *
     *  The default implementation throws Xapian::UnimplementedError.
     */
    virtual PositionList * open_position_list() const;

    /** Advance the current position to the next document in the postlist.
     *
     *  The list starts before the first entry in the list, so next(),
     *  skip_to() or check() must be called before any methods which need the
     *  context of the current position.
     *
     *  @param w_min	The minimum weight contribution that is needed (this is
     *			just a hint which PostList subclasses may ignore).
     *
     *  @return	If a non-NULL pointer is returned, then the caller should
     *		substitute the returned pointer for its pointer to us, and then
     *		delete us.  This "pruning" can only happen for a non-leaf
     *		subclass of this class.
     */
    virtual PostList* next(double w_min) = 0;

    /** Skip forward to the specified docid.
     *
     *  If the specified docid isn't in the list, position ourselves on the
     *  first document after it (or at_end() if no greater docids are present).
     *
     *  @param w_min	The minimum weight contribution that is needed (this is
     *			just a hint which PostList subclasses may ignore).
     *
     *  @return	If a non-NULL pointer is returned, then the caller should
     *		substitute the returned pointer for its pointer to us, and then
     *		delete us.  This "pruning" can only happen for a non-leaf
     *		subclass of this class.
     */
    virtual PostList* skip_to(Xapian::docid did, double w_min) = 0;

    /** Check if the specified docid occurs in this postlist.
     *
     *  The caller is required to ensure that the specified @a docid actually
     *  exists in the database.
     *
     *  This method acts like skip_to() if that can be done at little extra
     *  cost, in which case it then sets @a valid to true.
     *
     *  Otherwise it simply checks if a particular docid is present.  If it
     *  is, @a valid is set to true.  If it isn't, it sets @a valid to
     *  false, and leaves the position unspecified (and hence the result of
     *  calling methods which depend on the current position, such as
     *  get_docid() and at_end(), are also unspecified).  In this state, next()
     *  will advance to the first matching position after @a docid, and
     *  skip_to() will act as it would if the position was the first matching
     *  position after @a docid.  If @a valid is set to false, then NULL must
     *  be returned (pruning in this situation doesn't make sense).
     *
     *  The default implementation calls skip_to().
     */
    virtual PostList* check(Xapian::docid did, double w_min, bool &valid);

    /** Advance the current position to the next document in the postlist.
     *
     *  Any weight contribution is acceptable.
     */
    PostList* next() { return next(0.0); }

    /** Skip forward to the specified docid.
     *
     *  Any weight contribution is acceptable.
     */
    PostList* skip_to(Xapian::docid did) { return skip_to(did, 0.0); }

    /// Count the number of leaf subqueries which match at the current position.
    virtual Xapian::termcount count_matching_subqs() const;

    /// Gather PositionList* objects for a subtree.
    virtual void gather_position_lists(OrPositionList* orposlist);

    /** Get the bounds on the range of docids this PostList can return.
     *
     *  @param[out] first   Set to a lower bound on the docids that can be
     *			    returned, or not changed if there's no known
     *			    lower bound (other than 1).
     *
     *  @param[out] last    Set to an upper bound on the docids that can be
     *			    returned, or not changed if there's no known
     *			    upper bound (other than the highest used docid).
     *
     *  The default implementation (PostList::get_docid_range()) does nothing,
     *  which is suitable when there's no known lower or upper bound.
     */
    virtual void get_docid_range(docid& first, docid& last) const;

    /// Return a string description of this object.
    virtual std::string get_description() const = 0;
};

}
}

using Xapian::Internal::PostList;

#endif // XAPIAN_INCLUDED_POSTLIST_H
