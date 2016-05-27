#include <config.h>

#include <xapian/mset.h>

#include "omenquireinternal.h"
#include <xapian/document.h>

namespace Xapian {

MSet::MSet() { }

MSet::MSet(const MSet & o) : internal(o.internal) { }

MSet&
MSet::operator=(const MSet & o)
{
    internal = o.internal;
    return *this;
}

MSet::~MSet() { }

void
MSet::fetch_(Xapian::doccount first, Xapian::doccount last) const
{
    (void)first;
    (void)last;
}

int
MSet::convert_to_percent(double wt) const
{
    (void)wt;
    return 100;
}

Xapian::doccount
MSet::size() const
{
    return 0;
}

Xapian::doccount
MSet::get_matches_lower_bound() const
{
    return 0;
}

Xapian::doccount
MSet::get_matches_estimated() const
{
    return 0;
}

Xapian::doccount
MSet::get_matches_upper_bound() const
{
    return 0;
}

Xapian::doccount
MSet::get_termfreq(const std::string & term) const
{
    (void)term;
    return 0;
}

double
MSet::get_termweight(const std::string & term) const
{
    (void)term;
    return 0;
}

Xapian::doccount
MSet::get_firstitem() const
{
    return 0;
}

double
MSet::get_max_attained() const
{
    return 0;
}

double
MSet::get_max_possible() const
{
    return 0;
}

Xapian::doccount
MSet::get_uncollapsed_matches_lower_bound() const
{
    return 0;
}

Xapian::doccount
MSet::get_uncollapsed_matches_estimated() const
{
    return 0;
}

Xapian::doccount
MSet::get_uncollapsed_matches_upper_bound() const
{
    return 0;
}

std::string
MSet::snippet(const std::string & text,
	      size_t length,
	      const Xapian::Stem & stemmer,
	      unsigned flags,
	      const std::string & hi_start,
	      const std::string & hi_end,
	      const std::string & omit) const
{
    (void)length;
    (void)stemmer;
    (void)flags;
    (void)hi_start;
    (void)hi_end;
    (void)omit;
    return text;
}

std::string
MSet::get_description() const
{
    return "MSet()";
}


Xapian::Document
MSetIterator::get_document() const
{
    return Xapian::Document();
}

Xapian::docid
MSetIterator::operator*() const
{
    return 1;
}

double
MSetIterator::get_weight() const
{
    return 0;
}

std::string
MSetIterator::get_collapse_key() const
{
    return std::string();
}

Xapian::doccount
MSetIterator::get_collapse_count() const
{
    return 0;
}

std::string
MSetIterator::get_description() const
{
    return "MSet()";
}

}
