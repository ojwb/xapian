/** @file trainletor.cc
 * @brief Command line tool to prepare Letor training file and train the model
 */
/* Copyright (C) 2004,2005,2006,2007,2008,2009,2010,2015 Olly Betts
 * Copyright (C) 2011 Parth Gupta
 * Copyright (C) 2016 Ayush Tomar
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
#include <xapian-letor.h>

#include <cstdlib>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <set>
#include <math.h>

#include "gnu_getopt.h"

using namespace std;

#define PROG_NAME "letor-train"
#define PROG_DESC "Command line tool to prepare letor training file and train the model"

static void show_usage() {
    cout << "Usage: " PROG_NAME " [OPTIONS]\n"
"Options:\n"
"  --db=DIRECTORY  database to search (multiple databases may be specified)\n"
"  --query=PATH  path to query file\n"
"  --qrel=PATH  path to qrel file\n"
"  --msize=MSIZE   maximum number of matches to return\n"
"  --train=PATH  path to file by which training file is to be saved\n"
"  --model=PATH  path to file by training file is to be saved\n"
"  --help          display this help and exit\n"
"  --version       output version information and exit\n";
}

int
main(int argc, char **argv)
try {
    const char * opts = "a:b:c:d:e:f:g:h";
    static const struct option long_opts[] = {
    { "db",     required_argument, 0, 'a' },
    { "query",     required_argument, 0, 'b' },
    { "qrel",     required_argument, 0, 'c' },
    { "msize",  required_argument, 0, 'd' },
    { "train",     required_argument, 0, 'e' },
    { "model",     required_argument, 0, 'f' },
    { "help",   no_argument, 0, 'g' },
    { "version",    no_argument, 0, 'h' }
    };

    int msize = 10;

    bool have_database = false;

    char * query_path = NULL;
    char * qrel_path = NULL;
    char * training_path = NULL;
    char * model_path = NULL;

    Xapian::Database db;

    int c;
    while ((c = gnu_getopt_long(argc, argv, opts, long_opts, 0)) != -1) {
    switch (c) {
	case 'a':
	db.add_database(Xapian::Database(optarg));
	have_database = true;
	break;
	case 'b':
	query_path = optarg;
	break;
	case 'c':
	qrel_path = optarg;
	break;
	case 'd':
	msize = atoi(optarg);
	break;
	case 'e':
	training_path = optarg;
	break;
	case 'f':
	model_path = optarg;
	break;
	case 'g':
	cout << PROG_NAME " - " PACKAGE_STRING << endl;
	exit(0);
	case 'h':
	cout << PROG_NAME " - " PROG_DESC "\n\n";
	show_usage();
	exit(0);
	case ':': // missing parameter
	case '?': // unknown option
	show_usage();
	exit(1);
    }
    }

    if (!have_database) {
    cout << "No database specified so not running the query." << endl;
    exit(0);
    }

    // Initialise Letor object with db and ListNETRanker
    // If not explicitly passed as done below, the default ranker is used.
    // See Ranker documentation for available Ranker options.
    Xapian::Ranker * ranker = new Xapian::ListNETRanker();
    Xapian::Letor ltr(db, ranker);

    // Prepare the training file.
    ltr.prepare_training_file(query_path, qrel_path, msize, training_path);

    /// Learn and save the model.
    ltr.letor_learn_model(training_path, model_path);

    cout << flush;

} catch (const Xapian::Error & err) {
    cout << err.get_description() << endl;
    exit(1);
}
