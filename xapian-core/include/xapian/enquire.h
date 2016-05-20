#ifndef XAPIAN_INCLUDED_ENQUIRE_H
#define XAPIAN_INCLUDED_ENQUIRE_H

#include <xapian/mset.h>

namespace Xapian {
class Database;
class Query;

class ESetIterator {
  public:
    ESetIterator() { }
    double get_weight() const { return 0.0; }
    void operator++() { }
    void operator++(int) { }
    std::string operator*() { return std::string(); }
};

inline bool operator==(const ESetIterator&,const ESetIterator&) { return true; }
inline bool operator!=(const ESetIterator&,const ESetIterator&) { return false; }

class ESet {
  public:
    class Internal;
    ESet() { }
    ESetIterator begin() const { return ESetIterator(); }
    ESetIterator end() const { return ESetIterator(); }
};

class RSet {
  public:
    struct Internal { };
    void add_document(Xapian::docid) { }
    bool empty() const { return true; }
};

class Enquire {
  public:
    Enquire() { }
    Enquire(const Xapian::Database&) { }
    void set_query(const Xapian::Query&) { }
    MSet get_mset(Xapian::doccount, Xapian::doccount, const Xapian::RSet* = NULL) { return MSet(); }
    ESet get_eset(Xapian::doccount, const Xapian::RSet&) { return ESet(); }
};
}
#endif
