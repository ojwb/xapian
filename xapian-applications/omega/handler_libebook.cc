/** @file
 * @brief Extract text and metadata using libe-book.
 */
/* Copyright (C) 2019 Bruno Baruffaldi
 * Copyright (C) 2022,2023 Olly Betts
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301
 * USA
 */
#include <config.h>
#include "handler.h"
#include "stringutils.h"

#include <memory>

#include <librevenge-generators/librevenge-generators.h>
#include <librevenge-stream/librevenge-stream.h>
#include <libe-book/libe-book.h>

#define HANDLE_FIELD(START, END, FIELD, OUT...) \
   handle_field((START), (END), (FIELD), (CONST_STRLEN(FIELD)), OUT)

using libebook::EBOOKDocument;
using namespace librevenge;
using namespace std;

// Handle a field for which we only take a single value - we avoid copying in
// this case.
static void
handle_field(const char* start,
	     const char* end,
	     const char* field,
	     size_t len,
	     const char*& out,
	     size_t& out_len)
{
    if (size_t(end - start) > len && memcmp(start, field, len) == 0) {
	start += len;
	while (start != end && isspace(*start)) start++;
	if (start != end && (end[-1] != '\r' || --end != start)) {
	    out = start;
	    out_len = end - start;
	}
    }
}

// Handle a field for which we process multiple instances.  We just send each
// occurrence as we see it.
static void
handle_field(const char* start,
	     const char* end,
	     const char* field,
	     size_t len,
	     Field code)
{
    if (size_t(end - start) > len && memcmp(start, field, len) == 0) {
	start += len;
	while (start != end && isspace(*start)) start++;
	if (start != end && (end[-1] != '\r' || --end != start)) {
	    send_field(code, start, end - start);
	}
    }
}

static void
parse_metadata(const char* data, size_t len)
{
    const char* author;
    size_t author_len = 0;

    const char* p = data;
    const char* end = p + len;

    while (p != end) {
	const char* start = p;
	p = static_cast<const char*>(memchr(p, '\n', end - start));
	const char* eol;
	if (p)
	    eol = p++;
	else
	    p = eol = end;
	if ((end - start) > 5 && memcmp(start, "meta:", 5) == 0) {
	    start += 5;
	    switch (*start) {
		case 'i': {
		    // Use dc:creator in preference to meta:initial-creator.
		    if (!author_len)
			HANDLE_FIELD(start, eol, "initial-creator",
				     author, author_len);
		    break;
		}
		case 'k': {
		    HANDLE_FIELD(start, eol, "keyword", FIELD_KEYWORDS);
		    break;
		}
	    }
	} else if ((end - start) > 3 && memcmp(start, "dc:", 3) == 0) {
	    start += 3;
	    switch (*start) {
		case 'c': {
		    // Use dc:creator in preference to meta:initial-creator.
		    HANDLE_FIELD(start, eol, "creator", author, author_len);
		    break;
		}
		case 's': {
		    HANDLE_FIELD(start, eol, "subject", FIELD_KEYWORDS);
		    break;
		}
		case 't': {
		    HANDLE_FIELD(start, eol, "title", FIELD_TITLE);
		    break;
		}
	    }
	} else if ((end - start) > 8 && memcmp(start, "dcterms:", 8) == 0) {
	    start += 8;
	    HANDLE_FIELD(start, eol, "available", FIELD_KEYWORDS);
	}
    }

    if (author_len) {
	send_field(FIELD_AUTHOR, author, author_len);
    }
}

bool
initialise()
{
    return true;
}

void
extract(const string& filename, const string&)
{
    unique_ptr<RVNGInputStream> input;
    RVNGString dump, metadata;
    const char* file = filename.c_str();

    if (RVNGDirectoryStream::isDirectory(file))
	input.reset(new RVNGDirectoryStream(file));
    else
	input.reset(new RVNGFileStream(file));

    EBOOKDocument::Type type = EBOOKDocument::TYPE_UNKNOWN;
    auto confidence = EBOOKDocument::isSupported(input.get(), &type);

    if (confidence != EBOOKDocument::CONFIDENCE_EXCELLENT &&
	confidence != EBOOKDocument::CONFIDENCE_WEAK) {
	send_field(FIELD_ERROR, "Format not supported");
	return;
    }

    // Extract metadata.
    RVNGTextTextGenerator metadata_gen(metadata, true);
    if (EBOOKDocument::parse(input.get(), &metadata_gen, type) !=
	EBOOKDocument::RESULT_OK) {
	send_field(FIELD_ERROR, "Failed to extract metadata");
	return;
    }
    parse_metadata(metadata.cstr(), metadata.size());

    // Extract body text.
    RVNGTextTextGenerator content(dump, false);
    if (EBOOKDocument::parse(input.get(), &content, type) !=
	EBOOKDocument::RESULT_OK) {
	send_field(FIELD_ERROR, "Failed to extract text");
	return;
    }
    send_field(FIELD_BODY, dump.cstr(), dump.size());
}
