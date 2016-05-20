#ifndef XAPIAN_INCLUDED_MATCHER_OMENQUIREINTERNAL_H
#define XAPIAN_INCLUDED_MATCHER_OMENQUIREINTERNAL_H

#include <string>

#include <xapian/types.h>

namespace Xapian {
namespace Internal {

class MSetItem {
  public:
    std::string collapse_key;

    Xapian::docid did;

    double wt;

    MSetItem(unsigned, unsigned) { }
};

}
}
#endif
