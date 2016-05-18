#ifndef XAPIAN_INCLUDED_SELECTPOSTLIST_H
#define XAPIAN_INCLUDED_SELECTPOSTLIST_H

#include "api/postlist.h"

class SelectPostList : public PostList {
  protected:
    PostList* source;

  public:
    SelectPostList(PostList* source_)
	: source(source_)
    {
    }

    Xapian::doccount get_termfreq_min() const {
	// It's possible nothing matches.
	return 0;
    }

    Xapian::doccount get_termfreq_max() const {
	// It's possible everything in the source matches.
	return source->get_termfreq_max();
    }

    double get_maxweight() const {
	return source->get_maxweight();
    }

    Xapian::docid get_docid() const {
	return source->get_docid();
    }

    Xapian::termcount get_doclength() const {
	return source->get_doclength();
    }

    Xapian::termcount get_unique_terms() const {
	return source->get_unique_terms();
    }

    double get_weight() const {
	return source->get_weight();
    }

    bool at_end() const {
	return source == NULL;
    }

    double recalc_maxweight() {
	return source->recalc_maxweight();
    }

    Internal* next(double w_min) {
	/* FIXME */
	return source->next(w_min);
    }

    Internal* skip_to(Xapian::docid did, double w_min) {
	/* FIXME */
	return source->skip_to(did, w_min);
    }
};

#endif
