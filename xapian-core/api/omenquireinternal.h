#ifndef XAPIAN_INCLUDED_MATCHER_OMENQUIREINTERNAL_H
#define XAPIAN_INCLUDED_MATCHER_OMENQUIREINTERNAL_H

#include <string>

#include <xapian/enquire.h>
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
}
#endif
