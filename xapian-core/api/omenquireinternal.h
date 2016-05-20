#ifndef XAPIAN_INCLUDED_MATCHER_OMENQUIREINTERNAL_H
#define XAPIAN_INCLUDED_MATCHER_OMENQUIREINTERNAL_H

#include <string>
#include <vector>

#include <xapian/enquire.h>
#include <xapian/mset.h>
#include <xapian/types.h>

namespace Xapian {

namespace Internal {

class MSetItem {
  public:
    Xapian::docid did;

    double wt;

    std::string sort_key;

    std::string collapse_key;

    MSetItem(unsigned, unsigned) { }
};

}

class MSet::Internal : public Xapian::Internal::intrusive_base {
  public:
    Xapian::doccount matches_lower_bound;
    Xapian::doccount matches_estimated;
    Xapian::doccount matches_upper_bound;
    double max_possible;
    double max_attained;
    std::vector<Xapian::Internal::MSetItem> items;

    std::string get_description() const { return std::string(); }
};

}

#endif
