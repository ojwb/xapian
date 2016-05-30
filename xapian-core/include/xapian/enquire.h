#ifndef XAPIAN_INCLUDED_ENQUIRE_H
#define XAPIAN_INCLUDED_ENQUIRE_H

#include <xapian/database.h>
#include <xapian/eset.h>
#include <xapian/intrusive_ptr.h>
#include <xapian/mset.h>
#include <xapian/query.h>
#include <xapian/termiterator.h>

namespace Xapian {

class ExpandDecider;
class KeyMaker;
class MatchDecider;
class MatchSpy;
class RSet;

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
    enum {
	INCLUDE_QUERY_TERMS,
	USE_EXACT_TERMFREQ
    };
    Enquire() { }
    Enquire(const Xapian::Database&) { }
    void set_query(const Xapian::Query&, Xapian::termcount = 0) { }
    Query get_query() const { return Query(); }
    MSet get_mset(Xapian::doccount, Xapian::doccount, Xapian::doccount, const Xapian::RSet* = NULL, const Xapian::MatchDecider* = NULL) { return MSet(); }
    MSet get_mset(Xapian::doccount, Xapian::doccount, const Xapian::RSet* = NULL, const Xapian::MatchDecider* = NULL) { return MSet(); }
    ESet get_eset(Xapian::doccount, const Xapian::RSet&, const Xapian::ExpandDecider* = 0, double = 0.0) { return ESet(); }
    ESet get_eset(Xapian::doccount, const Xapian::RSet&, int, const Xapian::ExpandDecider*) { return ESet(); }
    ESet get_eset(Xapian::doccount, const Xapian::RSet&, int, double = 0.0, double = 0.0, int = 0) { return ESet(); }
    void set_expansion_scheme(const std::string&, double = 0.0) { }
    void set_weighting_scheme(const Xapian::Weight&) { }
    void set_collapse_key(Xapian::valueno, Xapian::doccount = 0) { }
    void set_cutoff(int, double = 0.0) { }
    void set_docid_order(Xapian::Enquire::docid_order) { }
    void set_sort_by_relevance() { }
    void set_sort_by_value(Xapian::valueno, bool) { }
    void set_sort_by_relevance_then_value(Xapian::valueno, bool) { }
    void set_sort_by_value_then_relevance(Xapian::valueno, bool) { }
    void set_sort_by_key(const Xapian::KeyMaker*, bool) { }
    void set_sort_by_relevance_then_key(const Xapian::KeyMaker*, bool) { }
    void set_sort_by_key_then_relevance(const Xapian::KeyMaker*, bool) { }
    TermIterator get_matching_terms_begin(Xapian::docid) const { return TermIterator(); }
    TermIterator get_matching_terms_end(Xapian::docid) const { return TermIterator(); }
    TermIterator get_matching_terms_begin(const Xapian::MSetIterator& it) const {
       return get_matching_terms_begin(*it);
    }
    TermIterator get_matching_terms_end(const Xapian::MSetIterator& it) const {
       return get_matching_terms_end(*it);
    }
    void add_matchspy(const Xapian::MatchSpy*) { }
    void clear_matchspies() { }
    void set_time_limit(double) { }
    std::string get_description() const { return "Enquire()"; }
};

}

#endif
