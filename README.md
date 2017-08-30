Key-Recovery Attacks on 5-round AES with a single Secret S-Box

Programs:

0) AES.c and AES_smallScaleVersion.c

Key-Recovery Attacks on AES with a single Secret S-Box:

1) AES_SecretSBox_AppH1.c

2) AES_SecretSBox_AppH2.c

3) AES_SecretSBox_AppH3.c

The programs should run with almost C-compilers (we have used gcc version 4.8.1). None of these programs is speed-optimized, they are for verification purposes only.

In all the programs, the main function is always the last one.

The first two programs contain our implementation of AES and of small-scale AES (encryption and decryption). They have been verified against test-vectors.

A complete description of this small scale variant of AES can be found in "Small Scale Variants of the AES" - Authors: C. Cid, S. Murphy, and M.J.B. Robshaw The program "AES_smallScaleVersion.c" provides an implementation of this small scale AES (verified against test-vectors).

The other three programs are the Key-Recovery Attacks on 5-round AES with a single Secret S-Box as described in App. H of the paper.
All the attacks have been verified on small-scale AES (due to the high complexity of the attacks for real AES).
However, since the properties that they exploit are independent of the fact that the words are composed of 4 or 8 bits, our verification on small-scale AES is strong evidence for it to hold for real AES.

For simplicity, the aim of all the attacks is to find part of the key.
However, the same procedure can be used to find all the key.

Both for "AES_SecretSBox_AppH1.c" and "AES_SecretSBox_AppH2.c", one can choose the implementation of these two attacks using the parameter IMPLEMENTATION (line 49).
In particular, if IMPLEMENTATION is equal to 0, then one uses the re-ordering algorithm. If IMPLEMENTATION is equal to 1, then one uses data-structure.
More details can be found in the paper.

1) "AES_SecretSBox_AppH1.c" verifies the attack on 5-round AES with a single Secret S-Box described in App. H.1. It exploits the fact that two coefficients of each row of the MixColumns matrix are equal.
The idea is to choose a set of plaintexts that depends on a guessed value of the key. It is possible to prove that the number of collisions among the ciphertexts in the same coset of M_J for |J|=3 is always even if the guessed key is the right one, while this happens with probability 50% if the guessed key is wrong.
The secret key can be chosen in the main function. The number of tests (or equivalent of initial sets used for the attack) can be chosen by the parameter NUMBER_TEST (line 46) - we suggest to choose NUMBER_TEST >=3 to have a probability to discard all the wrong keys higher than 95%.
Time of execution: 15 sec - IMPLEMENTATION = 0;
Time of execution: 45 sec - IMPLEMENTATION = 1.

2) "AES_SecretSBox_AppH2.c" verifies the attack on 5-round AES with a single Secret S-Box described in App. H.2. It exploits the fact that the sum of three coefficients of each row of the MixColumns matrix is equal to zero.
The idea is to choose a set of plaintexts that depends on a guessed value of the key. It is possible to prove that the number of collisions among the ciphertexts in the same coset of M_J for |J|=3 is always a multiple of 4 if the guessed key is the right one, while this happens with probability 50% if the guessed key is wrong.
The secret key can be chosen in the main function. The number of tests (or equivalent of initial sets used for the attack) can be chosen by the parameter NUMBER_TEST (line 46) - we suggest to choose NUMBER_TEST >=2 to have a probability to discard all the wrong keys higher than 95%.
Time of execution: 3.5 min - IMPLEMENTATION = 0.
Time of execution: 8 min - IMPLEMENTATION = 1.

3) "AES_SecretSBox_AppH3.c" verifies the attack on 5-round AES with a single Secret S-Box described in App. H.3. It exploits the fact that the sum of three coefficients of each row of the MixColumns matrix is equal to zero (in a different way w.r.t. "AES_SecretSBox_AppF2.c").
The idea is to choose a set of plaintexts that depends on a guessed value of the key. It is possible to prove that the number of collisions among the ciphertexts in the same coset of M_J for |J|=3 is always a multiple of 2 if the guessed key is the right one, while this happens with probability 50% if the guessed key is wrong.
The secret key can be chosen in the main function. The number of tests (or equivalent of initial sets used for the attack) can be chosen by the parameter NUMBER_TEST (line 46) - we suggest to choose NUMBER_TEST >=11 to have a probability to discard all the wrong keys higher than 95%.
Time of execution: 40 sec.

Finally, the pseudo-random generator used in these programs is the "Mersenne Twister" one, developed by 1997 by Makoto Matsumoto and Takuji Nishimura - MT19937. The complete source code and explanation of this random generator can be found in: http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/emt.html
