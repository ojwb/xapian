#ifndef XAPIAN_INCLUDED_MATCHER_ORPOSTLIST_H
#define XAPIAN_INCLUDED_MATCHER_ORPOSTLIST_H

#include "api/postlist.h"

class MultiMatch;

class OrPostList : public PostList {
  public:
    OrPostList(PostList*, PostList*, MultiMatch*, Xapian::doccount) { }

    Xapian::doccount get_termfreq_min() const { return 0; }

    Xapian::doccount get_termfreq_max() const { return 0; }

    Xapian::doccount get_termfreq_est() const { return 0; }

    double get_maxweight() const { return 0; }

    Xapian::docid get_docid() const { return 0; }

    Xapian::termcount get_doclength() const { return 0; }

    Xapian::termcount get_unique_terms() const { return 0; }

    double get_weight() const { return 0; }

    bool at_end() const { return 0; }

    double recalc_maxweight() { return 0; }

    Internal * next(double) { return 0; }

    Internal * skip_to(Xapian::docid, double) { return 0; }

    std::string get_description() const { return std::string(); }
};

#endif
