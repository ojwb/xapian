#ifndef XAPIAN_INCLUDED_BACKENDS_DOCUMENT_H
#define XAPIAN_INCLUDED_BACKENDS_DOCUMENT_H

#include <xapian/document.h>
#include <xapian/intrusive_ptr.h>

#include "backends/database.h"

namespace Xapian {
    class Document::Internal : public Xapian::Internal::intrusive_base {
      public:
	Xapian::Internal::intrusive_ptr<Xapian::Database::Internal> database;

	Xapian::docid did;

	std::string data;

	typedef std::map<Xapian::valueno, std::string> document_values;
	document_values values;

	std::string get_value(Xapian::valueno) { return std::string(); }
	void do_get_all_values(std::map<Xapian::valueno, std::string>& v) { v = values; }

	const std::string& do_get_data() { return data; }

	Internal(Xapian::Internal::intrusive_ptr<Xapian::Database::Internal>, bool) { }
    };
};

#endif
