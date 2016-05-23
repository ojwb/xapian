#include <config.h>

#include <xapian/rset.h>
#include "api/rsetinternal.h"

namespace Xapian {

RSet::RSet() { }

RSet::RSet(const RSet& o) : internal(o.internal) { }

RSet::~RSet() { }

RSet&
RSet::operator=(const RSet& o)
{
    internal = o.internal;
    return *this;
}

void
RSet::add_document(Xapian::docid did)
{
    internal->items.insert(did);

}

Xapian::doccount
RSet::size() const
{
    return internal->items.size();
}

}
