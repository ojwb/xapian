#ifndef XAPIAN_INCLUDED_DATABASE_H
#define XAPIAN_INCLUDED_DATABASE_H
#include <string>

#include <xapian/termiterator.h>

namespace Xapian {
struct Database {
    struct Internal {
	mutable unsigned _refs;
	unsigned get_doccount() const { return 0; }
	unsigned get_lastdocid() const { return 0; }
    };
    unsigned get_doccount() const { return 0; }
    double get_avlength() const { return 0; }
    unsigned get_collection_freq(const std::string&) const { return 0; }
    unsigned get_termfreq(const std::string&) const { return 0; }
    bool term_exists(const std::string&) const { return false; }
    std::string get_spelling_suggestion(const std::string&) const { return std::string(); }
    TermIterator synonyms_begin(const std::string&) const { return TermIterator(); }
    TermIterator synonyms_end(const std::string&) const { return TermIterator(); }
    TermIterator synonym_keys_begin(const std::string&) const { return TermIterator(); }
    TermIterator synonym_keys_end(const std::string&) const { return TermIterator(); }
};
struct WritableDatabase : public Database {
    void add_spelling(const std::string&) { }
};
}
#endif
