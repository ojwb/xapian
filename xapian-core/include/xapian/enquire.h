#ifndef XAPIAN_INCLUDED_ENQUIRE_H
#define XAPIAN_INCLUDED_ENQUIRE_H

#include <xapian/database.h>
#include <xapian/intrusive_ptr.h>
#include <xapian/mset.h>
#include <xapian/query.h>
#include <xapian/termiterator.h>

namespace Xapian {

class RSet;

class ExpandDecider;

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

class Enquire {
  public:
#ifndef SWIG
    class Internal {
      public:
	enum sort_setting { REL = 0, VAL = 1, VAL_REL = 2, REL_VAL = 3 };
	Xapian::Database db;
	Xapian::Query get_query() const { return Xapian::Query(); }
    };
#endif

    typedef enum {
	ASCENDING,
	DESCENDING,
	DONT_CARE
    } docid_order;
    Enquire() { }
    Enquire(const Xapian::Database&) { }
    void set_query(const Xapian::Query&) { }
    MSet get_mset(Xapian::doccount, Xapian::doccount, Xapian::doccount, const Xapian::RSet* = NULL) { return MSet(); }
    MSet get_mset(Xapian::doccount, Xapian::doccount, const Xapian::RSet* = NULL) { return MSet(); }
    ESet get_eset(Xapian::doccount, const Xapian::RSet&, const Xapian::ExpandDecider* = 0) { return ESet(); }
    void set_expansion_scheme(const std::string&, double = 0.0) { }
    void set_weighting_scheme(const Xapian::Weight&) { }
    void set_collapse_key(Xapian::valueno) { }
    void set_cutoff(double) { }
    void set_docid_order(Xapian::Enquire::docid_order) { }
    void set_sort_by_relevance_then_value(Xapian::valueno, bool) { }
    void set_sort_by_value_then_relevance(Xapian::valueno, bool) { }
    TermIterator get_matching_terms_begin(Xapian::docid) const { return TermIterator(); }
    TermIterator get_matching_terms_end(Xapian::docid) const { return TermIterator(); }
};

}

#endif
