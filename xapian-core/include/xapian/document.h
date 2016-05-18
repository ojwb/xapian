#ifndef XAPIAN_INCLUDED_DOCUMENT_H
#define XAPIAN_INCLUDED_DOCUMENT_H
#include <string>
#include <xapian/types.h>

namespace Xapian {
struct Document {
    struct Internal {
    };
    void add_posting(const std::string&, Xapian::termpos, Xapian::termcount) { }
    void add_term(const std::string&, Xapian::termcount) { }
    std::string get_description() const { return "Document()"; }
};
}
#endif
