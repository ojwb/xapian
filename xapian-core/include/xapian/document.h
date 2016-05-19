#ifndef XAPIAN_INCLUDED_DOCUMENT_H
#define XAPIAN_INCLUDED_DOCUMENT_H
#include <map>
#include <string>
#include <xapian/intrusive_ptr.h>
#include <xapian/types.h>
#include <xapian/visibility.h>

namespace Xapian {

class XAPIAN_VISIBILITY_DEFAULT Document {
  public:
    class Internal;
    Xapian::Internal::intrusive_ptr<Internal> internal;
    Document();
    Document(const Document& o);
    Document& operator=(const Document& o);
    ~Document();
    void add_posting(const std::string&, Xapian::termpos, Xapian::termcount) { }
    void add_term(const std::string&, Xapian::termcount) { }
    std::string get_value(Xapian::valueno) const { return std::string(); }
    std::string get_data() const { return std::string(); }
    void set_data(const std::string&) { }
    std::string get_description() const { return "Document()"; }
};

}
#endif
