/** @file quest.cc
 * @brief Command line search tool using Xapian::QueryParser.
 */
/* Copyright (C) 2004,2005,2006,2007,2008,2009,2010,2012,2013,2014,2016,2018,2019 Olly Betts
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

#include <xapian.h>

#include <benchmark/benchmark.h>

#include <climits>
#include <cstdlib>
#include <cstring>

#include <algorithm>
#include <iostream>

#include "gnu_getopt.h"

using namespace std;

#define PROG_NAME "quest"
#define PROG_DESC "Xapian command line search tool"

string folderName = "inbox";

static void BM_query_parse(benchmark::State& state) {
    for (auto _ : state) {
	Xapian::QueryParser queryparser;
        queryparser.add_boolean_prefix("folder", "XFOLDER:");
        queryparser.add_boolean_prefix("flag", "XF");

        string queryString = "folder:\"";
        queryString += folderName;
        queryString += "\"";

	Xapian::Query query = queryparser.parse_query(
	    queryString + " AND NOT flag:seen",
	    Xapian::QueryParser::FLAG_DEFAULT | Xapian::QueryParser::FLAG_PARTIAL
	);
    }
}
BENCHMARK(BM_query_parse);

static void BM_query_build(benchmark::State& state) {
    for (auto _ : state) {
	Xapian::Query query("XFOLDER:" + folderName);
	query &= ~Xapian::Query("XFseen");
    }
}
BENCHMARK(BM_query_build);

static void BM_full_mset(benchmark::State& state) {
    Xapian::Database db("tests/.glass/db__etext");
    Xapian::Query query("the");
    query &= ~Xapian::Query("now");
    for (auto _ : state) {
	Xapian::Enquire enquire(db);
	enquire.set_query(query);
	Xapian::MSet mset = enquire.get_mset(0, UINT_MAX);
	(void)mset.size();
    }
}
BENCHMARK(BM_full_mset);

static void BM_check_at_least(benchmark::State& state) {
    Xapian::Database db("tests/.glass/db__etext");
    Xapian::Query query("the");
    query &= ~Xapian::Query("now");
    for (auto _ : state) {
	Xapian::Enquire enquire(db);
	enquire.set_query(query);
	Xapian::MSet mset = enquire.get_mset(0, 0, UINT_MAX);
	(void)mset.get_matches_estimated();
    }
}
BENCHMARK(BM_check_at_least);

BENCHMARK_MAIN();
