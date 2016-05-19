#include <config.h>

#include <xapian/document.h>
#include "backends/document.h"

namespace Xapian {

Document::Document() { }

Document::Document(const Document& o) : internal(o.internal) { }

Document&
Document::operator=(const Document& o)
{
    internal = o.internal;
    return *this;
}

Document::~Document() { }

}
