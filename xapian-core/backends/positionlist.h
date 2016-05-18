#ifndef XAPIAN_INCLUDED_POSITIONLIST_H
#define XAPIAN_INCLUDED_POSITIONLIST_H
#include <xapian/positioniterator.h>
namespace Xapian {
    struct PositionIterator::Internal {
	unsigned _refs;
	int index;
	void skip_to(int) { }
	bool at_end() const { return true; }
	unsigned get_size() const { return 0; }
	unsigned get_position() const { return 0; }
	void next() { }
    };
}
typedef Xapian::PositionIterator::Internal PositionList;
#endif
