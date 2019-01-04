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
MANN_a27 | 126 | | 2.5588 | | 28188.80
MANN_a9 | 16 | | <0.0001 | | 21.28
brock200_1 | 21 | | 0.0312 | | 3986.96
brock200_2 | 12 | | 0.0406 | | 10229.60
brock200_3 | 15 | | 0.0197 | | 3939.77
brock200_4 | 17 | | 0.2787 | | 49016.80
brock400_1 | 27 | | 5.6202 | | 376982.00
brock400_2 | 29 | | 1.14214 | | 77090.50
brock400_3 | 31 | | 0.5525 | | 37027.40
brock400_4 | 33 | | 0.2821 | | 18977.30
c-fat200-1 | 12 | | <0.0001 | | 22.68
c-fat200-2 | 24 | | 0.0001 | | 322.25
c-fat200-5 | 58 | | <0.0001 | | 102.14
c-fat500-1 | 14 | | <0.0001 | | 43.62
c-fat500-10 | 126 | | 0.0003 | | 286.51
c-fat500-2 | 26 | | <0.0001 | | 47.13
c-fat500-5 | 64 | | 0.0002 | | 308.55
gen200_p0.9_44 | 44 | | 0.0239 | | 1260.37
gen200_p0.9_55 | 55 | | 0.0106 | | 493.62
gen400_p0.9_55 | 55 | | 3.4978 | | 85222.80
gen400_p0.9_65 | 65 | | 0.0451 | | 1051.62
gen400_p0.9_75 | 75 | | 0.0295 | | 621.28
hamming10-2 | 512 | | 0.4772 | | 6139.35
hamming10-4 | 40 | | 0.0475 | | 830.58
hamming6-2 | 32 | | <0.0001 | | 38.84
hamming6-4 | 4 | | <0.0001 | | 3.51
hamming8-2 | 128 | | 0.0034 | | 356.08
hamming8-4 | 16 | | 0.0001 | | 24.52
johnson16-2-4 | 8 | | <0.0001 | | 7.00
johnson32-2-4 | 16 | | 0.0001 | | 15.00
johnson8-2-4 | 4 | | <0.0001 | | 3.00
johnson8-4-4 | 14 | | <0.0001 | | 16.43
keller4 | 11 | | 0.0002 | | 35.76
keller5 | 27 | | 0.1577 | | 5547.13
p_hat1000-1 | 10 | | 0.0029 | | 257.53
p_hat1000-2 | 46 | | 0.0223 | | 407.86
p_hat1000-3 | 68 | | 0.4249 | | 4416.48
p_hat1500-2 | 65 | | 0.0609 | | 536.09
p_hat1500-3 | 94 | | 0.8463 | | 4142.05
p_hat300-1 | 8 | | 0.0007 | | 121.98
p_hat300-2 | 25 | | 0.0009 | | 83.83
p_hat300-3 | 36 | | 0.0127 | | 627.98
p_hat500-1 | 9 | | 0.0008 | | 127.80
p_hat500-2 | 36 | | 0.0042 | | 181.63
p_hat500-3 | 50 | | 0.0814 | | 1889.70
p_hat700-1 | 11 | | 0.0129 | | 1669.86
p_hat700-2 | 44 | | 0.0079 | | 209.48
p_hat700-3 | 62 | | 0.0287 | | 388.26
san200_0.7_1 | 30 | | 0.0269 | | 2919.57
san200_0.7_2 | 18 | | 0.1174 | | 14513.90
san200_0.9_1 | 70 | | 0.0195 | | 690.30
san200_0.9_2 | 60 | | 0.0190 | | 791.09
san200_0.9_3 | 44 | | 0.0255 | | 1345.60
san400_0.5_1 | 13 | | 0.3185 | | 38764.10
san400_0.7_1 | 40 | | 1.5214 | | 76995.00
san400_0.7_2 | 30 | | 1.1743 | | 73826.80
san400_0.7_3 | 22 | | 0.5864 | | 43450.10
san400_0.9_1 | 100 | | 0.3744 | | 5896.68
sanr200_0.7 | 18 | | 0.0045 | | 584.85
sanr200_0.9 | 42 | | 0.0949 | | 5488.88
sanr400_0.5 | 13 | | 0.0374 | | 5246.33
sanr400_0.7 | 21 | | 0.0440 | | 3614.38

