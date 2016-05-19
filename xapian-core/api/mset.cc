#include <config.h>

#include <xapian/mset.h>

#include <xapian/document.h>

namespace Xapian {

class MSet::Internal : public Xapian::Internal::intrusive_base {
};

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
MSet::get_matches_estimated() const
{
    return 0;
}

Xapian::doccount
MSet::get_firstitem() const
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
