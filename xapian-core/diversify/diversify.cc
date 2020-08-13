/** @file diversify.cc
 *  @brief Diversification API
 */
/* Copyright (C) 2018 Uppinder Chugh
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

#include "xapian/mset.h"
#include "api/msetinternal.h"

#include "xapian/cluster.h"
#include "xapian/error.h"
#include "xapian/types.h"

#include "debuglog.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <map>
#include <unordered_map>
#include <vector>

using namespace Xapian;
using namespace std;

void
MSet::diversify(Xapian::doccount k,
		Xapian::doccount r,
		double lambda,
		double b,
		double sigma_sqr)
{
    internal->diversify(k, r, lambda, b, sigma_sqr);
}

class Diversify {
    /// Copies are not allowed
    Diversify(const Diversify&) = delete;

    /// Assignment is not allowed
    void operator=(const Diversify&) = delete;

    /// Numerator used in MPT calculations.
    double numerator;

    /// Store each document from given mset as a point
    std::unordered_map<Xapian::docid, Xapian::Point> points;

    /// Store the relevance score of each document
    std::unordered_map<Xapian::docid, double> scores;

    /// Store pairwise cosine similarities of documents of given mset
    std::map<std::pair<Xapian::docid, Xapian::docid>, double> pairwise_sim;

  public:
    /// Map docid to MSet index.
    std::unordered_map<Xapian::docid, Xapian::doccount> mset_index;

    /// Store docids of top k diversified documents
    std::vector<Xapian::docid> main_dmset;

    /// Constructor.
    explicit Diversify(double b, double sigma_sqr)
	: numerator(2.0 * b * sigma_sqr) {}

    /** Initialise diversified document set
     *
     *  Convert top-k documents of mset into vector of Points, which
     *  represents the initial diversified document set.
     *
     *  @param k	The number of documents to diversify
     *  @param source	MSet object containing the documents of which
     *			top-k are to be diversified
     */
    void initialise_points(Xapian::doccount k,
			   const Xapian::MSet& source);

    /** Compute pairwise similarities
     *
     *  Used for pre-computing pairwise cosine similarities of documents
     *  of given mset, which is used to speed up evaluate_dmset
     *
     *  @param cset	Cluster of given relevant documents
     */
    void compute_similarities(const Xapian::ClusterSet& cset);

    /** Evaluate a diversified mset
     *
     *  Evaluate a diversified mset using MPT algorithm
     *
     *  @param dmset	Set of points representing candidate diversifed
     *			set of documents
     *  @param cset	Set of clusters of given mset
     *  @param lambda	MPT parameter
     */
    double evaluate_dmset(const std::vector<Xapian::docid>& dmset,
			  const Xapian::ClusterSet& cset,
			  double lambda);

    /// Return diversified document set from given mset
    Xapian::DocumentSet get_dmset(const Xapian::MSet& mset);
};

void
Diversify::initialise_points(Xapian::doccount k,
			     const MSet& source)
{
    TermListGroup tlg(source);
    Xapian::doccount count = 0;
    for (MSetIterator it = source.begin(); it != source.end(); ++it) {
	Xapian::docid did = *it;
	mset_index.emplace(did, count);
	points.emplace(did, Xapian::Point(tlg, it.get_document()));
	scores[did] = it.get_weight();
	// Initial top-k diversified documents
	if (++count <= k) {
	    main_dmset.push_back(did);
	}
    }
}

/** Return a key for a pair of documents
 *
 *  Returns a key as a pair of given documents ids
 *
 *  @param doc_id	Document id of the document
 *  @param centroid_idx	Index of cluster to which the given centroid
 *  			belongs to in the cluster set
 */
static inline pair<Xapian::docid, unsigned int>
get_key(Xapian::docid doc_id, unsigned int centroid_id)
{
    return make_pair(doc_id, centroid_id);
}

void
Diversify::compute_similarities(const Xapian::ClusterSet& cset)
{
    Xapian::CosineDistance d;
    for (auto p : points) {
	Xapian::docid point_id = p.first;
	Xapian::Point point = p.second;
	for (unsigned int c = 0; c < cset.size(); ++c) {
	    double dist = d.similarity(point, cset[c].get_centroid());
	    auto key = get_key(point_id, c);
	    pairwise_sim[key] = dist;
	}
    }
}

double
Diversify::evaluate_dmset(const vector<Xapian::docid>& dmset,
			  const Xapian::ClusterSet& cset,
			  double lambda)
{
    double score_1 = 0, score_2 = 0;

    for (auto doc_id : dmset)
	score_1 += scores[doc_id];

    for (unsigned int c = 0; c < cset.size(); ++c) {
	double min_dist = numeric_limits<double>::max();
	unsigned int pos = 1;
	for (auto doc_id : dmset) {
	    auto key = get_key(doc_id, c);
	    double sim = pairwise_sim[key];
	    double weight = numerator / log(1 + pos) * (1 - sim);
	    min_dist = min(min_dist, weight);
	    ++pos;
	}
	score_2 += min_dist;
    }

    return -lambda * score_1 + (1 - lambda) * score_2;
}

void
MSet::Internal::diversify(Xapian::doccount k,
			  Xapian::doccount r,
			  double lambda,
			  double b,
			  double sigma_sqr)
{
    if (r == 0)
	throw InvalidArgumentError("Value of r should be greater than zero");

    // Leave MSet alone if there's no need to diversify.
    if (k == 0 || items.size() <= 2) {
	return;
    }

    if (k > items.size())
	k = items.size();

    Diversify diversifier(b, sigma_sqr);
    diversifier.initialise_points(k, MSet(this));

    // Cluster the given mset into k clusters
    Xapian::LCDClusterer lc(k);
    Xapian::ClusterSet cset = lc.cluster(MSet(this));
    diversifier.compute_similarities(cset);

    // topC contains union of top-r relevant documents of each cluster
    vector<Xapian::docid> topc;

    // Build topC
    for (unsigned int c = 0; c < cset.size(); ++c) {
	auto documents = cset[c].get_documents();
	for (unsigned int d = 0; d < r && d < documents.size(); ++d) {
	    auto doc_id = documents[d].get_docid();
	    topc.push_back(doc_id);
	}
    }

    vector<Xapian::docid> curr_dmset = diversifier.main_dmset;

    while (true) {
	bool found_better_dmset = false;
	for (unsigned int i = 0; i < diversifier.main_dmset.size(); ++i) {
	    auto curr_doc = diversifier.main_dmset[i];
	    double best_score = diversifier.evaluate_dmset(curr_dmset, cset,
							   lambda);
	    bool found_better_doc = false;

	    for (unsigned int j = 0; j < topc.size(); ++j) {
		// Continue if candidate document from topC already
		// exists in curr_dmset
		auto candidate_doc = find(curr_dmset.begin(), curr_dmset.end(),
					  topc[j]);
		if (candidate_doc != curr_dmset.end()) {
		    continue;
		}

		auto temp_doc = curr_dmset[i];
		curr_dmset[i] = topc[j];
		double score = diversifier.evaluate_dmset(curr_dmset, cset,
							  lambda);

		if (score < best_score) {
		    curr_doc = curr_dmset[i];
		    best_score = score;
		    found_better_doc = true;
		}

		curr_dmset[i] = temp_doc;
	    }
	    if (found_better_doc) {
		curr_dmset[i] = curr_doc;
		found_better_dmset = true;
	    }
	}

	// Terminate algorithm when there's no change in current
	// document matchset
	if (!found_better_dmset)
	    break;

	diversifier.main_dmset = curr_dmset;
    }

    // Reorder the items in the MSet - those selected as diverse go first.
    vector<Result> diversified_results;
    diversified_results.reserve(items.size());
    for (auto did : diversifier.main_dmset) {
	Xapian::doccount idx = diversifier.mset_index[did];
	diversified_results.push_back(std::move(items[idx]));
	// Make sure such items get their docid zeroed, for the loop below.
	items[idx] = Result(0, 0.0);
    }

    if (k < items.size()) {
	// Then the others make up the remainder of the MSet, in the same
	// relative order as they were originally.
	for (auto& result : items) {
	    if (result.get_docid()) {
		diversified_results.push_back(std::move(result));
	    }
	}
    }

    items = std::move(diversified_results);
}
