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

}
