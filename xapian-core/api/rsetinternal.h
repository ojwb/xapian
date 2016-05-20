#ifndef XAPIAN_INCLUDED_API_RSETINTERNAL_H
#define XAPIAN_INCLUDED_API_RSETINTERNAL_H

#include <set>

#include <xapian/rset.h>
#include <xapian/types.h>

namespace Xapian {

class RSet::Internal : public Xapian::Internal::intrusive_base {
  public:
    std::set<Xapian::docid> items;

    const std::set<Xapian::docid>& get_items() const { return items; }
};

}

#endif
