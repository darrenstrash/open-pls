# Comparison with Pullan's PLS

Pullan's original algorithm was designed to be run on DIMACS graphs, which are generally small and dense. As such, his implementation probably used an adjacency matrix. However, this limits the applicability of the algorithm to large sparse networks, which are often of interest in practice. Our implementation is designed for sparse graphs, and uses an adjacency list. While this enables it to handle a more diverse collection of data sets, using an adjacency list can lead to slow solving on dense graphs, such as those in Pullan's paper.


### Criteria

To illustrate the algorithm performance differences, we ran experiments the compare the present implementation to Pullan's. There are two criteria to compare:

- *Selections*: the number of times a new vertex is selected for insertion into the approximate clique. 

- *Running time*: how long (in wall clock time) the algorithm runs.

### Experimental Setup

We repeat the same experiment as Pullan from the following paper:

*Phased local search for the maximum clique problem*,  
**Wayne Pullan**,  
Journal of Combinatorial Optimization, 12 (3), pp. 303–323, 2006  
[doi:10.1007/s10878-006-9635-y](https://doi.org/10.1007/s10878-006-9635-y)

For each data set, we give a *target* clique size, using the `--target-weight` command-line option, and run the algorithm over 100 trials with random seeds `0` through `99`, computing the average number of selections and the average running time in seconds. 

Experiments were run on a Macbook Pro with a 2.3 GHz Intel Core i5, which has a similar speed to Pullan's (2.4 GHz Pentium IV) machine.

### Table

Graph                | Clique Number |     Time (Pullan) | Time (Ours) | Selections (Pullan) | Selections (Ours)
:----                | ------------: |          -------: | --------:   |              -----: | -----:
C125.9               |            34 |            0.0001 |   0.0024    |                199  | 191.85
C250.9 | 44 | | 0.0447 | | 2051.35

MANN_a27 | 126 | | 2.55877 | | 28188.8
MANN_a9 | 16 | | <0.0001 | | 21.28
brock200_1 | 21 | | 0.031275 | | 3986.96
brock200_2 | 12 | | 0.040634 | | 10229.6
brock200_3 | 15 | | 0.019743 | | 3939.77
brock200_4 | 17 | | 0.278674 | | 49016.8
brock400_1 | 27 | | 376982 | | 5.62016
brock400_2 | 29 | | 1.14214 | | 77090.5
brock400_3 | 31 | | 0.552496 | | 37027.4
brock400_4 | 33 | | 0.2821 | | 18977.3


