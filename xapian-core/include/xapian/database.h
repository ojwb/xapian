#ifndef XAPIAN_INCLUDED_DATABASE_H
#define XAPIAN_INCLUDED_DATABASE_H

#include <iosfwd>
#include <string>
#include <vector>

#include <xapian/document.h>
#include <xapian/error.h>
#include <xapian/intrusive_ptr.h>
#include <xapian/positioniterator.h>
#include <xapian/postingiterator.h>
#include <xapian/termiterator.h>
#include <xapian/types.h>
#include <xapian/valueiterator.h>
#include <xapian/visibility.h>

namespace Xapian {

class Compactor;

class XAPIAN_VISIBILITY_DEFAULT Database {
    /// Internal helper behind public compact() methods.
    void compact_(const std::string * output_ptr,
		  int fd,
		  unsigned flags,
		  int block_size,
		  Xapian::Compactor * compactor) const;

  public:
    class Internal;
    std::vector<Xapian::Internal::intrusive_ptr<Internal> > internal;
    Database();
    Database(const Database& o);
    Database& operator=(const Database& o);
    ~Database();
    explicit Database(Internal*);
    explicit Database(const std::string& path, int flags = 0);
    explicit Database(int fd, int flags = 0);
    void add_database(const Database&) { }
    Xapian::doccount get_doccount() const { return 0; }
    double get_avlength() const { return 0; }
    Xapian::termcount get_doclength(Xapian::docid) const { return 0; }
    Xapian::termcount get_doclength_lower_bound() const { return 0; }
    Xapian::termcount get_doclength_upper_bound() const { return 0; }
    Xapian::termcount get_wdf_upper_bound(const std::string&) const { return 0; }
    Xapian::termcount get_unique_terms(Xapian::docid) const { return 0; }
    Xapian::termcount get_collection_freq(const std::string&) const { return 0; }
    Xapian::doccount get_termfreq(const std::string&) const { return 0; }
    bool term_exists(const std::string&) const { return false; }
    std::string get_spelling_suggestion(const std::string&, int = 0) const { return std::string(); }
    TermIterator allterms_begin(const std::string& = std::string()) const { return TermIterator(); }
    TermIterator allterms_end(const std::string& = std::string()) const { return TermIterator(); }
    TermIterator metadata_keys_begin(const std::string& = std::string()) const { return TermIterator(); }
    TermIterator metadata_keys_end(const std::string& = std::string()) const { return TermIterator(); }
    PositionIterator positionlist_begin(Xapian::docid, const std::string&) const { stub(); return PositionIterator(); }
    PositionIterator positionlist_end(Xapian::docid, const std::string&) const { return PositionIterator(); }
    PostingIterator postlist_begin(const std::string&) const { return PostingIterator(); }
    PostingIterator postlist_end(const std::string&) const { return PostingIterator(); }
    TermIterator spellings_begin() const { return TermIterator(); }
    TermIterator spellings_end() const { return TermIterator(); }
    TermIterator synonyms_begin(const std::string&) const { return TermIterator(); }
    TermIterator synonyms_end(const std::string&) const { return TermIterator(); }
    TermIterator synonym_keys_begin(const std::string& = std::string()) const { return TermIterator(); }
    TermIterator synonym_keys_end(const std::string& = std::string()) const { return TermIterator(); }
    TermIterator termlist_begin(Xapian::docid) const { stub(); return TermIterator(); }
    TermIterator termlist_end(Xapian::docid) const { return TermIterator(); }
    ValueIterator valuestream_begin(Xapian::valueno) const { return ValueIterator(); }
    ValueIterator valuestream_end(Xapian::valueno) const { return ValueIterator(); }
    Xapian::doccount get_value_freq(Xapian::valueno) const { return 0; }
    std::string get_value_lower_bound(Xapian::valueno) const { return std::string(); }
    std::string get_value_upper_bound(Xapian::valueno) const { return std::string(); }
    Xapian::Document get_document(Xapian::docid) const { return Xapian::Document(); }
    std::string get_metadata(const std::string&) const { return std::string(); }
    void compact(const std::string& output,
		 unsigned flags,
		 int block_size,
		 Xapian::Compactor& compactor) const {
	compact_(&output, 0, flags, block_size, &compactor);
    }
    void compact(const std::string& output,
		 unsigned flags = 0,
		 int block_size = 0) const {
	compact_(&output, 0, flags, block_size, NULL);
    }
    void compact(int fd,
		 unsigned flags,
		 int block_size,
		 Xapian::Compactor& compactor) const {
	compact_(NULL, fd, flags, block_size, &compactor);
    }

    void compact(int fd,
		 unsigned flags = 0,
		 int block_size = 0) {
	compact_(NULL, fd, flags, block_size, NULL);
    }
    Xapian::docid get_lastdocid() const { return 0; }
    std::string get_uuid() const { return std::string(); }
    bool has_positions() const { return false; }
    bool reopen() { return false; }
    void close() { }
    void keep_alive() { }
    std::string get_description() const { return "Database()"; }
    static size_t check(const std::string&, int = 0, std::ostream* = 0) { return 0; }
    static size_t check(int, int = 0, std::ostream* = 0) { return 0; }
};

struct XAPIAN_VISIBILITY_DEFAULT WritableDatabase : public Database {
    WritableDatabase() { }
    WritableDatabase(const std::string&, int flags, int blocksize = 0);
    void add_spelling(const std::string&, Xapian::termcount = 1) { }
    void remove_spelling(const std::string&, Xapian::termcount = 1) { }
    Xapian::docid add_document(const Xapian::Document&) { return 1; }
    void replace_document(Xapian::docid, const Xapian::Document&) { }
    Xapian::docid replace_document(const std::string&, const Xapian::Document&) { return 1; }
    void delete_document(Xapian::docid) { }
    void delete_document(const std::string&) { }
    void add_synonym(const std::string&, const std::string&) { }
    void remove_synonym(const std::string&, const std::string&) { }
    void clear_synonyms(const std::string&) { }
    void set_metadata(const std::string&, const std::string&) { }
    void commit() { }
    void flush() { commit(); }
    void begin_transaction(bool = true) { }
    void commit_transaction() { }
    void cancel_transaction() { }
};

}

#endif
