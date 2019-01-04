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
c-fat200-1 | 12 | | 0 | | 22.68
c-fat200-2 | 24 | | 0.000135 | | 322.25
c-fat200-5 | 58 | | 6e-05 | | 102.14
c-fat500-1 | 14 | | 1.9e-05 | | 43.62
c-fat500-10 | 126 | | 0.000265 | | 286.51
c-fat500-2 | 26 | | 1.3e-05 | | 47.13
c-fat500-5 | 64 | | 0.000175 | | 308.55
gen200_p0.9_44 | 44 | | 0.023861 | | 1260.37
gen200_p0.9_55 | 55 | | 0.010611 | | 493.62
gen400_p0.9_55 | 55 | | 3.4978 | | 85222.8
gen400_p0.9_65 | 65 | | 0.045101 | | 1051.62
gen400_p0.9_75 | 75 | | 0.029485 | | 621.28
hamming10-2 | 512 | | 0.477166 | | 6139.35
hamming10-4 | 40 | | 0.047479 | | 830.58
hamming6-2 | 32 | | 8.9e-05 | | 38.84
hamming6-4 | 4 | | 0 | | 3.51
hamming8-2 | 128 | | 0.003426 | | 356.08
hamming8-4 | 16 | | 0.000111 | | 24.52
johnson16-2-4 | 8 | | 7 | | 0 
johnson32-2-4 | 16 | | 15 | | 0.0001
johnson8-2-4 | 4 | | 3  | | 0
johnson8-4-4 | 14 | | 1.4e-05 | | 16.43
keller4 | 11 | | 0.00017 | | 35.76
keller5 | 27 | | 0.157686 | | 5547.13
p_hat1000-1 | 10 | | 0.002858 | | 257.53
p_hat1000-2 | 46 | | 0.02226 | | 407.86
p_hat1000-3 | 68 | | 0.424892 | | 4416.48
p_hat1500-2 | 65 | | 0.060926 | | 536.09
p_hat1500-3 | 94 | | 0.846284 | | 4142.05
p_hat300-1 | 8 | | 0.000667 | | 121.98
p_hat300-2 | 25 | | 0.000944 | | 83.83
p_hat300-3 | 36 | | 0.012742 | | 627.98
p_hat500-1 | 9 | | 0.000785 | | 127.8
p_hat500-2 | 36 | | 0.004223 | | 181.63
p_hat500-3 | 50 | | 0.081447 | | 1889.7
p_hat700-1 | 11 | | 0.012942 | | 1669.86
p_hat700-2 | 44 | | 0.007882 | | 209.48
p_hat700-3 | 62 | | 0.028731 | | 388.26
san200_0.7_1 | 30 | | 0.026912 | | 2919.57
san200_0.7_2 | 18 | | 0.11749 | | 14513.9
san200_0.9_1 | 70 | | 0.019484 | | 690.3
san200_0.9_2 | 60 | | 0.019002 | | 791.09
san200_0.9_3 | 44 | | 0.025531 | | 1345.6
san400_0.5_1 | 13 | | 0.31851 | | 38764.1
san400_0.7_1 | 40 | | 76995 | | 1.52137
san400_0.7_2 | 30 | | 1.17427 | | 73826.8
san400_0.7_3 | 22 | | 0.586399 | | 43450.1
san400_0.9_1 | 100 | | 0.374425 | | 5896.68
sanr200_0.7 | 18 | | 0.004453 | | 584.85
sanr200_0.9 | 42 | | 0.094898 | | 5488.88
sanr400_0.5 | 13 | | 0.037355 | | 5246.33
sanr400_0.7 | 21 | | 0.04402 | | 3614.38

