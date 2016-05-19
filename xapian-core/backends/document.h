#ifndef XAPIAN_INCLUDED_BACKENDS_DOCUMENT_H
#define XAPIAN_INCLUDED_BACKENDS_DOCUMENT_H

#include <xapian/document.h>
#include <xapian/intrusive_ptr.h>

namespace Xapian {
    class Document::Internal : public Xapian::Internal::intrusive_base {
      public:
	typedef std::map<Xapian::valueno, std::string> document_values;
	document_values values;
    };
};

#endif
