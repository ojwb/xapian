#ifndef XAPIAN_INCLUDED_API_OMENQUIREINTERNAL_H
#define XAPIAN_INCLUDED_API_OMENQUIREINTERNAL_H

#include <string>
#include <vector>

#include <xapian/enquire.h>
#include <xapian/mset.h>
#include <xapian/types.h>

#include "weight/weightinternal.h"

namespace Xapian {

namespace Internal {

class MSetItem {
  public:
    double wt;

    Xapian::docid did;

    std::string sort_key;

    std::string collapse_key;

    Xapian::doccount collapse_count;

    MSetItem(double wt_,
	     Xapian::docid did_,
	     const std::string& collapse_key_ = std::string(),
	     Xapian::doccount collapse_count_ = 0)
	: wt(wt_),
	  did(did_),
	  collapse_key(collapse_key_),
	  collapse_count(collapse_count_) { }
};

}

class MSet::Internal : public Xapian::Internal::intrusive_base {
  public:
    Xapian::doccount matches_lower_bound;
    Xapian::doccount matches_estimated;
    Xapian::doccount matches_upper_bound;
    Xapian::doccount uncollapsed_lower_bound;
    Xapian::doccount uncollapsed_estimated;
    Xapian::doccount uncollapsed_upper_bound;
    double max_possible;
    double max_attained;
    double percent_factor;
    Xapian::Weight::Internal* stats;
    std::vector<Xapian::Internal::MSetItem> items;
    Xapian::doccount firstitem;
    Xapian::Enquire::Internal* enquire;

    Internal(Xapian::doccount firstitem_,
	     Xapian::doccount matches_upper_bound_,
	     Xapian::doccount matches_lower_bound_,
	     Xapian::doccount matches_estimated_,
	     Xapian::doccount uncollapsed_upper_bound_,
	     Xapian::doccount uncollapsed_lower_bound_,
	     Xapian::doccount uncollapsed_estimated_,
	     double max_possible_,
	     double max_attained_,
	     const std::vector<Xapian::Internal::MSetItem>& items_,
	     double percent_factor_)
	: matches_lower_bound(matches_lower_bound_),
	  matches_estimated(matches_estimated_),
	  matches_upper_bound(matches_upper_bound_),
	  uncollapsed_lower_bound(uncollapsed_lower_bound_),
	  uncollapsed_estimated(uncollapsed_estimated_),
	  uncollapsed_upper_bound(uncollapsed_upper_bound_),
	  max_possible(max_possible_),
	  max_attained(max_attained_),
	  percent_factor(percent_factor_),
	  items(items_),
	  firstitem(firstitem_) { }

    std::string snippet(const std::string& text,
			size_t length,
			const Xapian::Stem& stemmer,
			unsigned flags,
			const std::string& hi_start,
			const std::string& hi_end,
			const std::string& omit) const;

    std::string get_description() const { return std::string(); }
};

}

#endif
