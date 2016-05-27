#ifndef XAPIAN_INCLUDED_RSET_H
#define XAPIAN_INCLUDED_RSET_H

#include <xapian/intrusive_ptr.h>
#include <xapian/types.h>
#include <xapian/visibility.h>

#include <string>

namespace Xapian {

class XAPIAN_VISIBILITY_DEFAULT RSet {
  public:
    class Internal;
    Xapian::Internal::intrusive_ptr<Internal> internal;
    RSet();
    RSet(const RSet& o);
    ~RSet();
    RSet& operator=(const RSet& o);
    void add_document(Xapian::docid did);
    Xapian::doccount size() const;
    bool empty() const { return size() == 0; }
    std::string get_description() const { return "RSet()"; }
};

}

#endif
