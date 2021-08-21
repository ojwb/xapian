/** @file
 * @brief Generic parser for XML - just replaces tags with spaces
 */
/* Copyright (C) 2020 Olly Betts
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#ifndef OMEGA_INCLUDED_GENERICXMLPARSER_H
#define OMEGA_INCLUDED_GENERICXMLPARSER_H

#include "xmlparser.h"

class GenericXmlParser : public XmlParser {
  public:
    std::string dump;

    GenericXmlParser() { }
    void process_content(const std::string& content);
};

#endif // OMEGA_INCLUDED_GENERICXMLPARSER_H
