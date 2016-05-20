#ifndef XAPIAN_INCLUDED_BACKENDS_DATABASE_H
#define XAPIAN_INCLUDED_BACKENDS_DATABASE_H

#include <xapian/database.h>

#include "common/internaltypes.h"
#include "positionlist.h"
#include "api/termlist.h"
#include "valuelist.h"

class LeafPostList;

namespace Xapian {

class Database::Internal : public Xapian::Internal::intrusive_base {
  public:
    Xapian::doccount get_doccount() const { return 0; }
    Xapian::docid get_lastdocid() const { return 0; }
    totlen_t get_total_length() const { return 0; }
    Xapian::doclength get_avlength() const { return 0; }
    Xapian::termcount get_doclength(Xapian::docid) const { return 0; }
    Xapian::termcount get_doclength_lower_bound() const { return 0; }
    Xapian::termcount get_doclength_upper_bound() const { return 0; }
    Xapian::termcount get_wdf_upper_bound(const std::string&) const { return 0; }
    Xapian::termcount get_unique_terms(Xapian::docid) const { return 0; }
    void get_freqs(const std::string&,
		   Xapian::doccount* tf,
		   Xapian::termcount* cf) const { *tf = *cf = 0; }
    Xapian::doccount get_value_freq(Xapian::valueno) const { return 0; }
    std::string get_value_lower_bound(Xapian::valueno) const { return 0; }
    std::string get_value_upper_bound(Xapian::valueno) const { return std::string(); }
    bool term_exists(const std::string&) const { return false; }
    bool has_positions() const { return false; }
    LeafPostList * open_post_list(const std::string&) const { return 0; }
    ValueList * open_value_list(Xapian::valueno) const { return 0; }
    TermList* open_term_list(Xapian::docid) const { return 0; }
    TermList* open_allterms(const std::string&) const { return 0; }
    PositionList* open_position_list(Xapian::docid, const std::string&) const { return 0; }
    Xapian::Document::Internal* open_document(Xapian::docid, bool) const { return 0; }
    TermList* open_spelling_termlist(const std::string&) const { return 0; }
    TermList* open_spelling_wordlist() const { return NULL; }
    Xapian::doccount get_spelling_frequency(const std::string&) const { return 0; }
    TermList* open_synonym_termlist(const std::string&) const { return 0; }
    TermList* open_synonym_keylist(const std::string&) const { return 0; }
    std::string get_metadata(const std::string&) const { return std::string(); }
    TermList* open_metadata_keylist(const std::string&) const { return 0; }
    void request_document(Xapian::docid) const { }
    Xapian::Document::Internal* collect_document(Xapian::docid) const { return 0; }
    std::string get_revision_info() const { return std::string(); }
    std::string get_uuid() const { return std::string(); }
    void invalidate_doc_object(Xapian::Document::Internal*) const { }
    int get_backend_info(std::string* path) const { return !path; }
    void get_used_docid_range(Xapian::docid& first, Xapian::docid& last) { first = last = 0; }
};

}

#endif
