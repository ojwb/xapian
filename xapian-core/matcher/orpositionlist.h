/** @file orpositionlist.h
 * @brief Merge two PositionList objects using an OR operation.
 */
/* Copyright (C) 2007,2010,2017 Olly Betts
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

#ifndef XAPIAN_INCLUDED_ORPOSITIONLIST_H
#define XAPIAN_INCLUDED_ORPOSITIONLIST_H

#include "backends/positionlist.h"
#include "api/postlist.h"

#include "xapian/error.h"
#include <algorithm>
#include <vector>

class OrPositionList : public PositionList {
  protected:
    /// The PositionList objects we're merging.
    std::vector<PositionList*> pls;

    /// The current respective positions.
    std::vector<Xapian::termpos> current;

  public:
    OrPositionList() { }

    PositionList* gather(PostList* pl) {
	pls.clear();
	pl->gather_position_lists(pls);
	if (pls.size() == 1)
	    return pls[0];
	if (pls.size() > 2)
	    throw Xapian::UnimplementedError("position lists for OR with more than 2 subqueries");
	current.resize(pls.size());
	std::fill(current.begin(), current.end(), 0);
	return this;
    }

    Xapian::termcount get_size() const;

    Xapian::termpos get_position() const;

    void next();

    void skip_to(Xapian::termpos termpos);

    bool at_end() const;
};

#endif // XAPIAN_INCLUDED_ORPOSITIONLIST_H
