# Comparison with Pullan's PLS

Pullan's original algorithm was designed to be run on DIMACS graphs, which are generally small and dense. As such, his implementation problem used an adjacency matrix. However, this limits the applicability of the algorithm to large sparse networks, which are often of interest in practice.

### Criteria

We briefly compare the present implementation to that of Pullan. There are two criteria to compare:

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



