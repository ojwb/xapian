/** @file diversifyinternal.h
 * @brief Xapian::Diversify internals
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

#ifndef XAPIAN_INCLUDED_DIVERSIFYINTERNAL_H
#define XAPIAN_INCLUDED_DIVERSIFYINTERNAL_H

#include <xapian/cluster.h>
#include <xapian/types.h>

#include <map>
#include <unordered_map>
#include <vector>

class Diversify {
    /// Copies are not allowed
    Diversify(const Diversify&) = delete;

    /// Assignment is not allowed
    void operator=(const Diversify&) = delete;

    /// Top-k documents of given mset are diversified
    Xapian::doccount k;

    /// Number of relevant documents from each cluster used for building topC
    Xapian::doccount r;

    /// MPT parameters
    double lambda, b, sigma_sqr;

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

    /// Constructor for initialising diversification parameters
    explicit Diversify(Xapian::doccount k_,
		       Xapian::doccount r_,
		       double lambda_,
		       double b_,
		       double sigma_sqr_)
	: k(k_), r(r_), lambda(lambda_), b(b_), sigma_sqr(sigma_sqr_) {}

    /** Initialise diversified document set
     *
     *  Convert top-k documents of mset into vector of Points, which
     *  represents the initial diversified document set.
     *
     *  @param source	MSet object containing the documents of which
     *			top-k are to be diversified
     */
    void initialise_points(const Xapian::MSet& source);

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
     */
    double evaluate_dmset(const std::vector<Xapian::docid>& dmset,
			  const Xapian::ClusterSet& cset);

    /// Return diversified document set from given mset
    Xapian::DocumentSet get_dmset(const Xapian::MSet& mset);
};

#endif // XAPIAN_INCLUDED_DIVERSIFYINTERNAL_H
