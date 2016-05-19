#ifndef XAPIAN_INCLUDED_DOCUMENT_H
#define XAPIAN_INCLUDED_DOCUMENT_H
#include <string>
#include <xapian/types.h>

namespace Xapian {
struct Document {
    struct Internal {
    };
    Document() { }
    void add_posting(const std::string&, Xapian::termpos, Xapian::termcount) { }
    void add_term(const std::string&, Xapian::termcount) { }
    std::string get_value(Xapian::valueno) const { return std::string(); }
    std::string get_data() const { return std::string(); }
    void set_data(const std::string&) { }
    std::string get_description() const { return "Document()"; }
};
}
#endif
