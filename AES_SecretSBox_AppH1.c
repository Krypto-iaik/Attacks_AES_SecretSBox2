
/**The Random Generator used in this code is the "Mersenne Twister" one, developed by 1997 by Makoto Matsumoto
and Takuji Nishimura - MT19937.
The complete source code of the random generator can be found in http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/MT2002/CODES/mt19937ar.c
We also attach the following:
"A C-program for MT19937, with initialization improved 2002/1/26.
   Coded by Takuji Nishimura and Makoto Matsumoto.
   Before using, initialize the state by using init_genrand(seed)
   or init_by_array(init_key, key_length).
   Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
   All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
     1. Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.
     2. Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.
     3. The names of its contributors may not be used to endorse or promote
        products derived from this software without specific prior written
        permission.
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
   Any feedback is very welcome.
   http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/emt.html
   email: m-mat @ math.sci.hiroshima-u.ac.jp (remove space)"
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "AES_common.h"
#include "AES_smallScale_sbox.h"
#include "multiplication.h"
#include "subspace_checks.h"

#define NUMBER_CP 1048576
/**DON'T MODIFY THE PREVIOUS TWO NUMBERS!*/
#define NUMBER_TEST 3
/**The previous number denotes the number of tests = number of initial sets to use.
In order to have a probability higher than 95% to discard all the wrong keys, we suggest NUMBER_TEST >=3.*/
#define IMPLEMENTATION 0
/**There are two possible ways to implement such a distinguisher:
0 - using data-structure;
1 - using a re-ordering algorithm.*/

word8 cipher[NUMBER_CP][16], cipher2copy[NUMBER_CP][16];
int arrray[65536];

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

long int pow(int r, int e)
{
    int i;
    long int result = 1;

    if(e == 0)
        return 1;

    if(e < 0)
        return -1;

    for(i = 0; i<e; i++)
    {
        result = result * r;
    }

    return result;
}

int logarithm2(long int n)
{
    long int i = 1;
    int j = 1;

    if (n<=0)
        return -1;

    if (n == 1)
        return 0;

    while(i<n)
    {
        i = i * 2;
        j++;
    }

    return j;

}

long int pow2(int n)
{
    long int i = 1;
    int j;

    if(n == 0)
        return 1;

    for(j=0; j<n;j++)
    {
        i = i * 2;
    }

    return i;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**Merge-Sort Algorithm*/

/*given two ciphertexts a and b, it return 0 if a<=b, 1 otherwise */
int lessOrEqual(word8 a[], word8 b[], int coset)
{
    int i, temp, aaa, bbb;

    for(i=0;i<4;i++)
    {
        temp = 4*coset - 3 * i;

        if(temp < 0)
            temp = temp + 16;

        aaa = (int) a[temp];
        bbb = (int) b[temp];

        if(aaa>bbb)
            return 1;

        if(aaa<bbb)
            return 0;
    }

    return 0;
}

double merging(int low, int mid, int high, int coset, double numberTableLook) {

    int l1, l2, i, j;
    word8 text1[16], text2[16];

    l1 = low;
    l2 = mid;

    for(j = 0; j<16; j++)
    {
        text1[j] = cipher[l1][j];
        text2[j] = cipher[l2][j];
    }

    numberTableLook = numberTableLook + 2.0;

    for(i = low; ((l1 < mid) && (l2 < high)); i++)
    {

        numberTableLook = numberTableLook + 1.0;

        if(lessOrEqual(text1, text2, coset) == 0)
        {
            for(j = 0; j<16; j++)
            {
                cipher2copy[i][j] = text1[j];
            }
            l1++;
            for(j = 0; j<16; j++)
            {
                text1[j] = cipher[l1][j];
            }

        }
        else
        {
            for(j = 0; j<16; j++)
            {
                cipher2copy[i][j] = text2[j];
            }
            l2++;
            for(j = 0; j<16; j++)
            {
                text2[j] = cipher[l2][j];
            }
        }
    }

    while(l1 < mid)
    {
        numberTableLook = numberTableLook + 1.0;

       for(j = 0; j<16; j++)
        {
            cipher2copy[i][j] = cipher[l1][j];
        }
        i++;
        l1++;
    }

    while(l2 < high)
    {
        numberTableLook = numberTableLook + 1.0;

       for(j = 0; j<16; j++)
        {
            cipher2copy[i][j] = cipher[l2][j];
        }
        i++;
        l2++;
    }

    for(i = low; i < high; i++)
    {
        numberTableLook = numberTableLook + 1.0;

        for(j = 0; j<16; j++)
        {
            cipher[i][j] = cipher2copy[i][j];
        }
    }

    return numberTableLook;
}

double merging2(int low, int mid, int high, int coset, double numberTableLook) {

    int l1, l2, i, j;
    word8 text1[16], text2[16];

    l1 = low;
    l2 = mid;

    for(j = 0; j<16; j++)
    {
        text1[j] = cipher[l1][j];
        text2[j] = cipher[l2][j];
    }

    numberTableLook = numberTableLook + 2.0;

    for(i = low; ((l1 < mid) && (l2 < high)); i++)
    {
        numberTableLook = numberTableLook + 1.0;

        if(lessOrEqual(text1, text2, coset) == 0)
        {
            for(j = 0; j<16; j++)
            {
                cipher2copy[i][j] = text1[j];
            }
            l1++;
            for(j = 0; j<16; j++)
            {
                text1[j] = cipher[l1][j];
            }
        }
        else
        {
            for(j = 0; j<16; j++)
            {
                cipher2copy[i][j] = text2[j];
            }
            l2++;
            for(j = 0; j<16; j++)
            {
                text2[j] = cipher[l2][j];
            }
        }
    }

    while(l1 < mid)
    {
       numberTableLook = numberTableLook + 1.0;

       for(j = 0; j<16; j++)
        {
            cipher2copy[i][j] = cipher[l1][j];
        }
        i++;
        l1++;
    }

    while(l2 < high)
    {
        numberTableLook = numberTableLook + 1.0;

       for(j = 0; j<16; j++)
        {
            cipher2copy[i][j] = cipher[l2][j];
        }
        i++;
        l2++;
    }

    for(i = low; i < high; i++)
    {
        numberTableLook = numberTableLook + 1.0;

        for(j = 0; j<16; j++)
        {
            cipher[i][j] = cipher2copy[i][j];
        }
    }

    return numberTableLook;
}

double sort(int coset, double numberTableLook)
{
    int log, i, j, division, high, low, middle, a, b;
    word8 t1[16], t2[16];

    log = logarithm2(NUMBER_CP);

    for(i=0; i<NUMBER_CP; i = i+2)
    {
        numberTableLook = numberTableLook + 2.0;

        for(j=0;j<16;j++)
        {
            t1[j] = cipher[i][j];
            t2[j] = cipher[i+1][j];
        }

        if(lessOrEqual(t1, t2, coset) == 1)
        {
            for(j=0;j<16;j++)
            {
                cipher[i][j] = t2[j];
                cipher[i+1][j] = t1[j];
            }
            numberTableLook = numberTableLook + 2.0;
        }
    }

    for(i = 2; i < log; i++)
    {
        a = pow2(i);
        b = a/2;
        division = NUMBER_CP / a;

        for(j = 0; j < division; j++)
        {
            high = a * (j+1);
            low = a * j;
            middle = low + b;

            numberTableLook = merging(low, middle, high, coset, numberTableLook);

        }
    }

    return numberTableLook;

}

double sort2(int coset, double numberTableLook)
{
    int log, i, j, division, high, low, middle, a, b;
    word8 t1[16], t2[16];

    log = logarithm2(NUMBER_CP);

    for(i=0; i<NUMBER_CP; i = i+2)
    {
        for(j=0;j<16;j++)
        {
            t1[j] = cipher[i][j];
            t2[j] = cipher[i+1][j];
        }

        numberTableLook = numberTableLook + 2.0;

        if(lessOrEqual(t1, t2, coset) == 1)
        {
            for(j=0;j<16;j++)
            {
                cipher[i][j] = t2[j];
                cipher[i+1][j] = t1[j];
            }

            numberTableLook = numberTableLook + 2.0;
        }
    }

    for(i = 2; i < log; i++)
    {
        a = pow2(i);
        b = a/2;
        division = NUMBER_CP / a;

        for(j = 0; j < division; j++)
        {
            high = a * (j+1);
            low = a * j;
            middle = low + b;

            numberTableLook = merging2(low, middle, high, coset, numberTableLook);

        }
    }

    return numberTableLook;

}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**Given a set, it counts the number of collision among the ciphertexts after 5 rounds - using data structure.
For the AES case, it must be an even number*/

int subspaceTest_v2(word8 key[][4], word8 key2, double *cost)
{
    unsigned long int i, j, numero = 0;
    int l, num, k, indice1, indice2, indice3, indice4, indice5, coset, finalRes = 0, rTemp, flag = 0, boh;

    double numberTableLook = 0.0;

    word8 p1[4][4], c1[4][4], temp[4][4];

    for(k=0;k<4;k++)
    {
        for(l=0;l<4;l++)
            temp[l][k]=randomByte();
    }

    i = 0;

    //Generate the set which depends on the guessed key
    for(num = 0; ((num < NUMBER_TEST)&&(finalRes == 0)); num++)
    {
        for(k=0;k<4;k++)
        {
            for(l=0;l<4;l++)
                temp[l][k]=randomByte();
        }

        i = 0;

        for(indice1 =0; indice1<16; indice1++)
        {
            temp[0][0] = (word8) indice1;
            for(indice2=0; indice2 <16; indice2++)
            {
                temp[1][1] = (word8) indice2;
                for(indice3=0; indice3<16; indice3++)
                {
                    temp[2][2] = (word8) indice3;
                    for(indice4=0; indice4<16; indice4++)
                    {
                        temp[3][3] = (word8) indice4;
                        for(indice5=0; indice5<16; indice5++)
                        {
                            temp[0][1] = (word8) indice5;
                            temp[1][2] = temp[0][1] ^ key2;

                            encryption(temp, key, &(c1[0][0]));

                            for(k = 0; k<4; k++)
                            {
                                for(l=0;l<4;l++)
                                    cipher[i][k + l*4] = c1[k][l];
                            }

                            i++;
                        }
                    }
                }
            }
        }

        numero = 0;

        //Four times!
        for(coset = 0; ((coset < 4)&&(finalRes == 0)); coset++)
        {
            for(i = 0; i<65536; i++)
            {
                arrray[i] = 0;
            }

            for(i = 0; i<NUMBER_CP; i++)
            {
                numero = 0;

                numberTableLook++;

                for(j = 0; j<4; j++)
                {
                    boh = coset - j;
                    if(boh < 0)
                        boh = boh + 4;

                    numero = numero + pow(16, j) * cipher[i][j + 4 * boh];
                }

                arrray[numero]++;
                numberTableLook++;
            }

            numero = 0;

            for(i = 0; i<65536; i++)
            {
                rTemp = arrray[i];
                numberTableLook++;
                rTemp = rTemp * (rTemp - 1);
                numero = numero + (rTemp/ 2);
            }

	    //Print the number of collision for each M_J with |J|=3
            printf("%d) Test - M_", (num+1));
            for(j=0;j<4;j++)
            {
                if(j!= coset)
                {
                    printf("%d", j);
                    if(((j<3)&&(coset!=3))||((j<2)&&(coset==3)))
                        printf(",");
                }
            }
            printf(" - %d - %d\n", numero, (numero%2));

            if((numero%2) != 0)
            {
                finalRes = -1;
            }

        }
    }

    if(key2 == (key[0][1]^key[1][2]))
    {
        if(finalRes == -1)
            printf("\t SOMETHING WRONG...\n");
        else
            printf("\t POSSIBLE right KEY!!!\n");
    }

    *cost += numberTableLook;

    return finalRes;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**Given a set, it counts the number of collision among the ciphertexts after 5 rounds - using re-ordering algorithm.
For the AES case, it must be an even number*/

int subspaceTest(word8 key[][4], word8 key2, double *cost)
{
    unsigned long int i, j, numero = 0;
    int l, num, k, indice1, indice2, indice3, indice4, indice5, coset, finalRes = 0, rTemp, flag = 0;

    double numberTableLook = 0.0;

    word8 p1[4][4], c1[4][4], temp[4][4];

    for(k=0;k<4;k++)
    {
        for(l=0;l<4;l++)
            temp[l][k]=randomByte();
    }

    i = 0;

    //Generate the set which depends on the guessed key
    for(num = 0; ((num < NUMBER_TEST)&&(finalRes == 0)); num++)
    {
        for(k=0;k<4;k++)
        {
            for(l=0;l<4;l++)
                temp[l][k]=randomByte();
        }

        i = 0;

        for(indice1 =0; indice1<16; indice1++)
        {
            temp[0][0] = (word8) indice1;
            for(indice2=0; indice2 <16; indice2++)
            {
                temp[1][1] = (word8) indice2;
                for(indice3=0; indice3<16; indice3++)
                {
                    temp[2][2] = (word8) indice3;
                    for(indice4=0; indice4<16; indice4++)
                    {
                        temp[3][3] = (word8) indice4;
                        for(indice5=0; indice5<16; indice5++)
                        {
                            temp[0][1] = (word8) indice5;
                            temp[1][2] = temp[0][1] ^ key2;

                            encryption(temp, key, &(c1[0][0]));

                            for(k = 0; k<4; k++)
                            {
                                for(l=0;l<4;l++)
                                    cipher[i][k + l*4] = c1[k][l];
                            }

                            i++;
                        }
                    }
                }
            }
        }

        numero = 0;

        //Four times!
        for(coset = 0; ((coset < 4)&&(finalRes == 0)); coset++)
        {
            //re-order the ciphertexts!
            numberTableLook = sort2(coset, numberTableLook);

            //count the number of collision
            i = 0;
            rTemp = 0;

            numero = 0;

            numberTableLook = numberTableLook + 1.0;

            while(i < (NUMBER_CP-1))
            {
                rTemp = 1;
                j = i;
                flag = 0;

                do
                {
                    flag = 0;

                    numberTableLook = numberTableLook + 1.0;

                    for(k = 0; k<4; k++)
                    {
                        for(l = 0; l<4; l++)
                        {
                            p1[k][l] = cipher[j+1][k + l*4] ^ cipher[j][k + l*4];
                        }
                    }

                    if(belongToW_2(p1, coset) == 1)
                    {
                        rTemp = rTemp + 1;
                        flag = 1;
                        j = j + 1;
                    }
                }while(flag == 1);

                i = j + 1;
                rTemp = rTemp * (rTemp - 1);
                numero = numero + (rTemp/ 2);
            }

	    //Print the number of collision for each M_J with |J|=3
            printf("%d) Test - M_", (num+1));
            for(j=0;j<4;j++)
            {
                if(j!= coset)
                {
                    printf("%d", j);
                    if(((j<3)&&(coset!=3))||((j<2)&&(coset==3)))
                        printf(",");
                }
            }
            printf(" - %d - %d\n", numero, (numero%2));

            if((numero%2) != 0)
            {
                finalRes = -1;
            }

        }
    }

    if(key2 == (key[0][1]^key[1][2]))
    {
        if(finalRes == -1)
            printf("\t SOMETHING WRONG...\n");
        else
            printf("\t POSSIBLE right KEY!!!\n");
    }

    *cost += numberTableLook;

    return finalRes;

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**Key-recovery attack on AES with a single secret S-Box (see App. F.1 - implementation on small scale AES).
This attack exploits the fact that two coefficient of each row of the MixColumns matrix are equal.

General Idea:
given a set which depends on a guessed value delta, the number of collisions after 5 rounds in the same coset of M is a multiple of 2 if delta is equal to the secret key,
while this happens with probability 50% for wrong guessed value.
For simplicity, the attack recovers only one nibble of the key. However, the same procedure of attack allows to recover the entire key.*/

//TIME OF EXECUTION: 15 sec - IMPLEMENTATION = 0
//TIME OF EXECUTION: 45 sec - IMPLEMENTATION = 1

int main()
{
    word8 key[4][4] = {
        0x5, 0x3, 0x4, 0xc,
        0xd, 0x0, 0x2, 0xd,
        0x2, 0xf, 0xe, 0x3,
        0xf, 0x7, 0x9, 0x1
    };

    int k, j, flag = 0, res, i, flag2 = 0;

    srand (time(NULL));

    unsigned long init[4], length=4;

    double cost = 0.;

    word8 key2;

    cost = 0.;

    //words of 4 bits!
    for(k=0;k<4;k++)
    {
        for(j=0;j<4;j++)
            key[j][k] =  key[j][k] & 0x0f;
    }

    for(k=0;k<length;k++)
    {
        init[k] = rand();
    }
    init_by_array(init, length);

    printf("5-round KEY-RECOVERY ATTACK on AES with a SINGLE SECRET S-Box.\n\n");

    printf("Implementation of the attack described in App. F.1 on small scale AES - it exploits the fact that two coefficients of each row of MC are equal.\n");
    printf("For each test, the program generates texts and counts the total number of collisions. For the right key, this number is always even.\n\n");

    printf("It prints the following:\n");
    printf("Index J of mixed space M_J -\nNumber of Collisions -\nCollision mod 2\n\n");

    for(i = 0; i<16; i++)
    {
        key2 = (word8) i;

        printf("\nTesting 0x%x...\n", key2);

        if(IMPLEMENTATION == 0)
        {
            //using data-structure
            res = subspaceTest_v2(key, key2, &cost);
        }
        else
        {
            //using a re-ordering algorithm
            res = subspaceTest(key, key2, &cost);
        }

        if(key2 == (key[0][1]^key[1][2]))
        {
            if(res == 0)
            {
                flag = 1;
                printf("NOTE: It's the secret KEY!\n");
            }
        }

        if(res == 0)
            flag2++;
    }

    if(flag == 1)
    {
        if(flag2 == 1)
            printf("\nRIGHT KEY FOUND!\n");
        else
            printf("\nMore than a single key found, but the right key is among them!\n");
    }

    if(IMPLEMENTATION == 0)
    {
        //using data-structure
         printf("Total Cost: %.0f - Theoretical: 226 492 416 = 2^27.8\n", cost);
    }
    else
    {
        //using re-ordering
        printf("Total Cost: %.0f - Theoretical: 4 035 219 480 = 2^31.9\n", cost);
    }

    return 0;
}
