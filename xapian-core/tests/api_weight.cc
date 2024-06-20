/** @file
 * @brief tests of Xapian::Weight subclasses
 */
/* Copyright (C) 2004-2024 Olly Betts
 * Copyright (C) 2013 Aarsh Shah
 * Copyright (C) 2016 Vivek Pal
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

#include <config.h>

#include "api_weight.h"

#include <xapian.h>

#include "apitest.h"
#include "testutils.h"

using namespace std;

/// Basic test of using weighting schemes.
DEFINE_TESTCASE(weight1, backend) {
    Xapian::Database db(get_database("etext"));
    Xapian::Enquire enquire(db);
    Xapian::Enquire enquire_scaled(db);
    auto term = "robinson";
    Xapian::Query q{term};
    enquire.set_query(q);
    enquire_scaled.set_query(q * 15.0);
    auto expected_matches = db.get_termfreq(term);
    auto helper = [&](const Xapian::Weight& weight,
		      string_view name,
		      string_view params) {
	tout << name << '(' << params << ")\n";
	enquire.set_weighting_scheme(weight);
	enquire_scaled.set_weighting_scheme(weight);
	Xapian::MSet mset = enquire.get_mset(0, expected_matches + 1);
	TEST_EQUAL(mset.size(), expected_matches);
	if (name == "Xapian::BoolWeight") {
	    /* All weights should be zero. */
	    TEST_EQUAL(mset[0].get_weight(), 0.0);
	    TEST_EQUAL(mset.back().get_weight(), 0.0);
	} else if (name == "Xapian::CoordWeight") {
	    /* All weights should be 1 for a single term query. */
	    TEST_EQUAL(mset[0].get_weight(), 1.0);
	    TEST_EQUAL(mset.back().get_weight(), 1.0);
	} else if (!params.empty()) {
	    /* All weights should be equal with these particular parameters. */
	    TEST_NOT_EQUAL(mset[0].get_weight(), 0.0);
	    TEST_EQUAL(mset[0].get_weight(), mset.back().get_weight());
	} else {
	    TEST_NOT_EQUAL(mset[0].get_weight(), 0.0);
	    TEST_NOT_EQUAL(mset[0].get_weight(), mset.back().get_weight());
	}
	Xapian::MSet mset_scaled = enquire_scaled.get_mset(0, expected_matches);
	TEST_EQUAL(mset_scaled.size(), expected_matches);
	for (Xapian::doccount i = 0; i < expected_matches; ++i) {
	    TEST_EQUAL_DOUBLE(mset_scaled[i].get_weight(),
			      mset[i].get_weight() * 15.0);
	}
    };
#define TEST_WEIGHTING_SCHEME(W, ...) helper(W(__VA_ARGS__), #W, #__VA_ARGS__)
    TEST_WEIGHTING_SCHEME(Xapian::BoolWeight);
    TEST_WEIGHTING_SCHEME(Xapian::CoordWeight);
    TEST_WEIGHTING_SCHEME(Xapian::DLHWeight);
    TEST_WEIGHTING_SCHEME(Xapian::DPHWeight);
    TEST_WEIGHTING_SCHEME(Xapian::DiceCoeffWeight);
    TEST_WEIGHTING_SCHEME(Xapian::TradWeight);
    TEST_WEIGHTING_SCHEME(Xapian::BM25Weight);
    TEST_WEIGHTING_SCHEME(Xapian::BM25PlusWeight);
    TEST_WEIGHTING_SCHEME(Xapian::TfIdfWeight);
    TEST_WEIGHTING_SCHEME(Xapian::InL2Weight);
    TEST_WEIGHTING_SCHEME(Xapian::IfB2Weight);
    TEST_WEIGHTING_SCHEME(Xapian::IneB2Weight);
    TEST_WEIGHTING_SCHEME(Xapian::BB2Weight);
    TEST_WEIGHTING_SCHEME(Xapian::PL2Weight);
    TEST_WEIGHTING_SCHEME(Xapian::PL2PlusWeight);
    TEST_WEIGHTING_SCHEME(Xapian::LMWeight);
    // Regression test for bug fixed in 1.2.4.
    TEST_WEIGHTING_SCHEME(Xapian::BM25Weight, 0, 0, 0, 0, 1);
    /* As mentioned in the documentation, when parameter k is 0, wdf and
     * document length don't affect the weights.  Regression test for bug fixed
     * in 1.2.4.
     */
    TEST_WEIGHTING_SCHEME(Xapian::TradWeight, 0);
#undef TEST_WEIGHTING_SCHEME
}
