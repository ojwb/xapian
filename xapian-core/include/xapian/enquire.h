#ifndef XAPIAN_INCLUDED_ENQUIRE_H
#define XAPIAN_INCLUDED_ENQUIRE_H

#include <xapian/database.h>
#include <xapian/mset.h>
#include <xapian/query.h>

namespace Xapian {

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
    class Internal {
      public:
	enum sort_setting { REL = 0, VAL = 1, VAL_REL = 2, REL_VAL = 3 };
	Xapian::Database db;
	Xapian::Query get_query() const { return Xapian::Query(); }
    };
    Enquire() { }
    Enquire(const Xapian::Database&) { }
    void set_query(const Xapian::Query&) { }
    MSet get_mset(Xapian::doccount, Xapian::doccount, Xapian::doccount) { return MSet(); }
    MSet get_mset(Xapian::doccount, Xapian::doccount, const Xapian::RSet* = NULL) { return MSet(); }
    ESet get_eset(Xapian::doccount, const Xapian::RSet&) { return ESet(); }
    void set_weighting_scheme(const Xapian::Weight&) { }
};
}
#endif
