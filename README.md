# **OpenPLS**: An open phased local search (PLS) implementation

[![license](https://img.shields.io/badge/license-GPL%20v3.0-blue.svg)](http://www.gnu.org/licenses/)
[![Build Status](https://travis-ci.com/darrenstrash/open-pls.svg?branch=master)](https://travis-ci.com/darrenstrash/open-pls)

Phased local search (PLS) is a simple, yet powerful technique to compute approximate solutions to combinatorial optimization problems. This software was written with two goals in mind:

- Have a working (open) implementation of PLS for the maximum clique / independent set problems and their weighted variants.
- Have the implementation work on sparse graphs that cannot be stored with an adjacency matrix.

The original PLS algorithm was *not* designed for sparse graphs, and thus there is a speed difference between published results using PLS and this software. (See a comparison with published results [here](/comparison.md). Effort is underway to provide an option for dense networks, which is competetive with the original publications.

### About this implementation 

The purpose of this package is two-fold:

1. Faithfully implement the algorithms from the following two papers:

*Phased local search for the maximum clique problem*,  
**Wayne Pullan**,  
Journal of Combinatorial Optimization, 12 (3), pp. 303–323, 2006  
[doi:10.1007/s10878-006-9635-y](https://doi.org/10.1007/s10878-006-9635-y)

and

*Optimisation of unweighted/weighted maximum independent sets and minimum vertex covers*,  
**Wayne Pullan**,  
Discrete Optimization, 6 (2), pp. 214–219, 2009  
[doi:10.1016/j.disopt.2008.12.001](https://doi.org/10.1016/j.disopt.2008.12.001)

**As noted above, the present implementation is optimized for sparse graphs, using an adjacency list instead of an adjacency matrix. As such, the present algorithm is slower than Pullan's algorithm on dense graphs. See a comparison [here](/comparison.md).**

2. Provide exact reproducibility of experimental results using PLS in the paper:

*Temporal Map Labeling: A New Unified Framework with Experiments*,  
**L. Barth, B. Niedermann, M. Nöllenburg, and D. Strash**,  
Proceedings of the 24th ACM SIGSPATIAL International Conference on Advances in Geographic Information Systems (ACM SIGSPATIAL 2016)  
[arXiv:1609.06327](https://arxiv.org/abs/1609.06327)

For the version of this software that exactly reproduces the results from the above paper, download [v1.0](https://github.com/darrenstrash/open-pls/releases/tag/v1.0)

### This package includes:

 - C++ code for phased local search, to compute an approximate:
   - maximum (unweighted) clique,
   - maximum weight clique,
   - maximum (unweighted) independent set, or
   - maximum weight independent set.
 - Converters to extract the graph and weights from a graphml file and convert to the  METIS format and to a text file storing node weights
 - A .graphml data file for testing (see ./data)

Please feel free to contact me with any questions!

### Version
1.0beta

### Building

First, make sure you have clang 7.3.0 or g++ 5.3, then execute

```sh
$ git clone https://github.com/darrenstrash/open-pls.git
$ cd open-pls
$ make
```

### Running
```sh
$ ./bin/pls --algorithm=<mc|mwc|mis|mwis> [--weighted] [--use-weight-file] --input-file=<input graph in METIS format> [--timeout=<timeout in seconds>] [--random-seed=<integer random seed>]
```

or

```sh
$ ./run.sh
```

to convert the graphml file in ./data to the appropriate format and run the Maximim Weight Independent Set algorithm (mwis).

### Graph Format

First, note that loops and directed edges are not supported, and could lead to errors.

Currently, one format is supported:

 - **The unweighted METIS format**: Which consists of

   `<# vertices> <# edges> 1`

   followed by `<# vertices>` lines of space-separated vertices,  where the `i`-th line consists of 
   all neighbors of `i`. All vertices range from `1` to `<# vertices>`

For weights, a separate file must be provided in the same directory with your METIS graph, with the name <METIS file>.weights

 - This is a file consisting of one weight per line, where line *i* indicates the weight for vertex *i-1*

Copyright
----

Copyright (c) 2016 Darren Strash.


License
----

This code is released under the GNU Public License (GPL) 3.0.

To read the GPL 3.0, read the file COPYING in this directory.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>

Contact
----

**Darren Strash** (first name DOT last name AT gmail DOT com)
