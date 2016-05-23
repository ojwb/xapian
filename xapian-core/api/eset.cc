#include <config.h>

#include <xapian/eset.h>
#include <xapian/intrusive_ptr.h>

namespace Xapian {

class ESet::Internal : public Xapian::Internal::intrusive_base {
};

ESet::ESet() { }

ESet::ESet(const ESet & o) : internal(o.internal) { }

ESet&
ESet::operator=(const ESet & o)
{
    internal = o.internal;
    return *this;
}

ESet::~ESet() { }

Xapian::doccount
ESet::size() const
{
    return 0;
}

std::string
ESetIterator::operator*() const
{
    return std::string();
}

double
ESetIterator::get_weight() const
{
    return 0;
}

}
