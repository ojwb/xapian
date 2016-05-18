#ifndef OM_HGUARD_MULTIMATCH_H
#define OM_HGUARD_MULTIMATCH_H
#include <vector>
#include "api/omenquireinternal.h"
#include "xapian/query.h"
#include "xapian/weight.h"
class MultiMatch {
    private:
	/// Copying is not permitted.
	MultiMatch(const MultiMatch &);
	/// Assignment is not permitted.
	void operator=(const MultiMatch &);
    public:
	MultiMatch() { }
#if 0
	MultiMatch(const Xapian::Database &,
		   const Xapian::Query &,
		   Xapian::termcount,
		   const Xapian::RSet *,
		   Xapian::doccount,
		   Xapian::valueno,
		   int,
		   double,
		   Xapian::Enquire::docid_order,
		   Xapian::valueno,
		   Xapian::Enquire::Internal::sort_setting,
		   bool,
		   double,
		   Xapian::Weight::Internal &,
		   const Xapian::Weight *,
		   const vector<Xapian::Internal::opt_intrusive_ptr<Xapian::MatchSpy>> &,
		   bool,
		   bool) { throw false; }
	void get_mset(Xapian::doccount,
		      Xapian::doccount,
		      Xapian::doccount,
		      Xapian::MSet &,
		      Xapian::Weight::Internal &,
		      const Xapian::MatchDecider *,
		      const Xapian::KeyMaker *) { throw false; }
#endif
	void recalc_maxweight() { throw false; }
};
#endif
