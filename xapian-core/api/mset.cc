#include <config.h>

#include <xapian/mset.h>

#include "omenquireinternal.h"
#include <xapian/document.h>

namespace Xapian {

MSet::MSet() { }

MSet::MSet(const MSet & o) : internal(o.internal) { }

MSet&
MSet::operator=(const MSet & o)
{
    internal = o.internal;
    return *this;
}

MSet::~MSet() { }

Xapian::doccount
MSet::size() const
{
    return 0;
}

Xapian::doccount
MSet::get_matches_lower_bound() const
{
    return 0;
}

Xapian::doccount
MSet::get_matches_estimated() const
{
    return 0;
}

Xapian::doccount
MSet::get_matches_upper_bound() const
{
    return 0;
}

Xapian::doccount
MSet::get_firstitem() const
{
    return 0;
}

double
MSet::get_max_attained() const
{
    return 0;
}

double
MSet::get_max_possible() const
{
    return 0;
}

Xapian::doccount
MSet::get_uncollapsed_matches_lower_bound() const
{
    return 0;
}

Xapian::doccount
MSet::get_uncollapsed_matches_estimated() const
{
    return 0;
}

Xapian::doccount
MSet::get_uncollapsed_matches_upper_bound() const
{
    return 0;
}


Xapian::Document
MSetIterator::get_document() const
{
    return Xapian::Document();
}

Xapian::docid
MSetIterator::operator*() const
{
    return 1;
}

double
MSetIterator::get_weight() const
{
    return 0;
}

}
