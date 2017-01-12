/** @file orpositionlist.cc
 * @brief Merge two PositionList objects using an OR operation.
 */
/* Copyright (C) 2007,2010,2016,2017 Olly Betts
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

#include "orpositionlist.h"

#include "debuglog.h"

using namespace std;

Xapian::termcount
OrPositionList::get_size() const
{
    LOGCALL(EXPAND, Xapian::termcount, "OrPositionList::get_size", NO_ARGS);
    // This is actually the upper bound, but generally there's only one term
    // at each position, so it'll usually be correct too.
    Xapian::termcount size = 0;
    for (auto pl : pls) size += pl->get_size();
    RETURN(size);
}

Xapian::termpos
OrPositionList::get_position() const
{
    LOGCALL(EXPAND, Xapian::termpos, "OrPositionList::get_position", NO_ARGS);
    Xapian::termpos result = Xapian::termpos(-1);
    for (auto pos : current) result = std::min(result, pos);
    RETURN(result);
}

void
OrPositionList::next()
{
    LOGCALL_VOID(EXPAND, "OrPositionList::next", NO_ARGS);
    // If we've not started yet, both current[0] and current[1] will be 0,
    // which gets handled by calling next() on both, which is what we want to
    // do to get started.
    if (current[0] <= current[1]) {
	bool equal = (current[0] == current[1]);
	if (equal && current[0] == Xapian::termpos(-1)) {
	    // Both either at_end() or on the largest value and will be
	    // at_end() if we call next().
	    pls[0] = pls[1] = NULL;
	    return;
	}

	pls[0]->next();
	if (pls[0]->at_end()) {
	    current[0] = Xapian::termpos(-1);
	    pls[0] = NULL;
	} else {
	    current[0] = pls[0]->get_position();
	}
	if (!equal) return;
    }

    pls[1]->next();
    if (pls[1]->at_end()) {
	current[1] = Xapian::termpos(-1);
	pls[1] = NULL;
    } else {
	current[1] = pls[1]->get_position();
    }
}

void
OrPositionList::skip_to(Xapian::termpos termpos)
{
    LOGCALL_VOID(EXPAND, "OrPositionList::skip_to", termpos);
    // If we've not started yet, both current[0] and current[1] will be 0,
    // which gets handled by calling next() on both, which is what we want to
    // do to get started.
    if (current[0] <= current[1]) {
	bool equal = (current[0] == current[1]);
	if (equal && current[0] == Xapian::termpos(-1)) {
	    // Both either at_end() or on the largest value, so skip_to() is a
	    // no-op.
	    return;
	}

	pls[0]->skip_to(termpos);
	if (pls[0]->at_end()) {
	    current[0] = Xapian::termpos(-1);
	    pls[0] = NULL;
	} else {
	    current[0] = pls[0]->get_position();
	}
	if (!equal) return;
    }

    pls[1]->skip_to(termpos);
    if (pls[1]->at_end()) {
	current[1] = Xapian::termpos(-1);
	pls[1] = NULL;
    } else {
	current[1] = pls[1]->get_position();
    }
}

bool
OrPositionList::at_end() const
{
    LOGCALL(EXPAND, bool, "OrPositionList::at_end", NO_ARGS);
    RETURN(pls[0] == NULL && pls[1] == NULL);
}
