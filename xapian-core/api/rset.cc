#include <config.h>

#include <xapian/rset.h>

#include <xapian/error.h>
#include "api/rsetinternal.h"
#include "str.h"

using namespace std;

namespace Xapian {

RSet::RSet() : internal(new Internal) { }

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
    if (rare(did == 0))
	throw Xapian::InvalidArgumentError("Document ID 0 is not valid");
    internal->items.insert(did);
}

Xapian::doccount
RSet::size() const
{
    return internal->items.size();
}

string
RSet::get_description() const
{
    string desc = "RSet(RSet::Internal(";
    desc += str(internal->items.size());
    desc += "))";
    return desc;
}

}
