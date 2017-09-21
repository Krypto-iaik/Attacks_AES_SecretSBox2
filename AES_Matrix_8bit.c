#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX 4
#define MDS 0
//Invertible = 0 / MDS = 1

typedef unsigned char word8;

double totMatrix = 0., invMatrix = 0., eqCoef = 0., zeroSum = 0.;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*Multiplication per x*/

word8 multiplicationX(word8 byte)
{
  word8 bitTemp;

  bitTemp=byte>>7;
  byte=byte<<1;

  if(bitTemp==0)
    return byte;
  else
    return (byte^0x1B);

}

/*Multiplication byte per x^n*/

word8 multiplicationXN(word8 byte, int n){

  int i;

  for(i=0;i<n;i++)
    byte=multiplicationX(byte);

  return byte;

}

word8 superProduct(word8 a, word8 b)
{

    word8 res = 0x00, temp = 0x00;

    temp = b & 0x01;
    if(temp != 0)
    {
        res = res ^ a;
    }

    temp = b & 0x02;
    if(temp != 0)
    {
        res = res ^ multiplicationX(a);
    }

    temp = b & 0x04;
    if(temp != 0)
    {
        temp = multiplicationXN(a, 2);
        res = res ^ temp;
    }

    temp = b & 0x08;
    if(temp != 0)
    {
        temp = multiplicationXN(a, 3);
        res = res ^ temp;
    }

    temp = b & 0x10;
    if(temp != 0)
    {
        temp = multiplicationXN(a, 4);
        res = res ^ temp;
    }

    temp = b & 0x20;
    if(temp != 0)
    {
        temp = multiplicationXN(a, 5);
        res = res ^ temp;
    }

    temp = b & 0x40;
    if(temp != 0)
    {
        temp = multiplicationXN(a, 6);
        res = res ^ temp;
    }

    temp = b & 0x80;
    if(temp != 0)
    {
        temp = multiplicationXN(a, 7);
        res = res ^ temp;
    }

    return res;

}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int equalCoeff(int a, int b, int c, int d)
{
    if(a == b)
        return 1;

    if(a == c)
        return 1;

    if(a == d)
        return 1;

    if(b == c)
        return 1;

    if(d == b)
        return 1;

    if(d == c)
        return 1;

    return 0;
}

int sumCoeffZero(int a, int b, int c, int d)
{
    int res1;

    res1 = equalCoeff(a, b, c, d);

    if(res1 == 1)
        return 1;

    if((a ^ b ^ c) == 0x00)
        return 1;

    if((a ^ b ^ d) == 0x00)
        return 1;

    if((a ^ c ^ d) == 0x00)
        return 1;

    if((b ^ c ^ d) == 0x00)
        return 1;

    if((a ^ b ^ c ^ d) == 0x00)
        return 1;

    return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

word8 determ(word8 a[MAX][MAX], int n)
{
  int p, h, k, i, j;
  word8 det = 0x00, temp[MAX][MAX], prodd1, prodd2;

  if(n==1)
  {
    return a[0][0];
  }
  else if(n==2)
  {
    prodd1 = superProduct(a[0][0], a[1][1]);
    prodd2 = superProduct(a[0][1], a[1][0]);
    det = prodd1 ^ prodd2;
    return det;
  }
  else
  {
    for(p=0;p<n;p++)
    {
      h = 0;
      k = 0;
      for(i=1;i<n;i++)
      {
        for(j=0;j<n;j++)
        {
          if(j==p)
          {
            continue;
          }
          temp[h][k] = a[i][j];
          k++;
          if(k==(n-1))
          {
            h++;
            k = 0;
          }
        }
      }

      prodd2 = determ(temp,(n-1));
      prodd1 = superProduct(a[0][p], prodd2);
      det = det ^ prodd1;
    }
    return det;
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

word8 checkMDS2(word8 a[MAX][MAX])
{
    int i, j, k, h;
    word8 det = 0x00, temp[MAX][MAX];

    for(i = 0; i<4; i++)
    {
        for(j = (i+1); j<4; j++)
        {
            for(k = 0; k<4; k++)
            {
                for(h = (k+1); h<4; h++)
                {
                    temp[0][0] = a[i][k];
                    temp[0][1] = a[i][h];
                    temp[1][0] = a[j][k];
                    temp[1][1] = a[j][h];

                    det = determ(temp, 2);
                    if(det == 0x00)
                    {
                        return 0x00;
                    }
                }
            }
        }
    }

    return 0x11;

}

word8 checkMDS3(word8 a[MAX][MAX])
{
    int i, j, k, h, jj, hh;
    word8 det = 0x00, temp[MAX][MAX];

    for(i = 0; i<4; i++)
    {
        for(j = (i+1); j<4; j++)
        {
            for(jj = (j+1); jj<4; jj++)
            {
                for(k = 0; k<4; k++)
                {
                    for(h = (k+1); h<4; h++)
                    {
                        for(hh = (h+1); hh<4; hh++)
                        {
                            //1 row
                            temp[0][0] = a[i][k];
                            temp[0][1] = a[i][h];
                            temp[0][2] = a[i][hh];
                            //2 row
                            temp[1][0] = a[j][k];
                            temp[1][1] = a[j][h];
                            temp[1][2] = a[j][hh];
                            //3 row
                            temp[2][0] = a[jj][k];
                            temp[2][1] = a[jj][h];
                            temp[2][2] = a[jj][hh];

                            det = determ(temp, 3);
                            if(det == 0x00)
                            {
                                return 0x00;
                            }
                        }
                    }
                }
            }
        }
    }

    return 0x11;

}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void testMatrix()
{
    int aa, bb, cc, dd, ec;
    word8 a, b, c, d, det, matrix[4][4];

    for(aa = 0; aa<256; aa++)
    {
        a = (word8) aa;
        printf("aaaIN %d\n", aa);
        for(bb = 0; bb<256; bb++)
        {
            printf("%d\n", bb);
            b = (word8) bb;
            for(cc = 0; cc<256; cc++)
            {
                c = (word8) cc;
                for(dd = 0; dd<256; dd++)
                {
                    d = (word8) dd;

                    if((aa != 0) || (bb != 0) || (cc != 0) || (dd != 0))
                    //if((aa != 0) && (bb != 0) && (cc != 0) && (dd != 0))
                    {

                        //first row
                        matrix[0][0] = a;
                        matrix[0][1] = b;
                        matrix[0][2] = c;
                        matrix[0][3] = d;
                        //second row
                        matrix[1][0] = b;
                        matrix[1][1] = c;
                        matrix[1][2] = d;
                        matrix[1][3] = a;
                        //first row
                        matrix[2][0] = c;
                        matrix[2][1] = d;
                        matrix[2][2] = a;
                        matrix[2][3] = b;
                        //second row
                        matrix[3][0] = d;
                        matrix[3][1] = a;
                        matrix[3][2] = b;
                        matrix[3][3] = c;

                        totMatrix = totMatrix + 1;

                        det = determ(matrix, 4);

                        if(det != 0x00)
                        {
                            invMatrix = invMatrix + 1;

                            ec = equalCoeff(a, b, c, d);

                            if(ec == 1)
                            {
                                eqCoef = eqCoef + 1;
                                zeroSum = zeroSum + 1;
                            }
                            else
                            {
                                zeroSum = zeroSum + sumCoeffZero(a, b, c, d);
                            }
                        }

                    }
                }
            }
        }
    }

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void testMatrixMDS()
{
    int aa, bb, cc, dd, ec;
    word8 a, b, c, d, det, matrix[4][4];

    for(aa = 0; aa<256; aa++)
    {
        printf("aaa %d\n", aa);
        a = (word8) aa;
        for(bb = 0; bb<256; bb++)
        {
            printf("%d\n", bb);
            b = (word8) bb;
            for(cc = 0; cc<256; cc++)
            {
                c = (word8) cc;
                for(dd = 0; dd<256; dd++)
                {
                    d = (word8) dd;

//                    if((aa != 0) || (bb != 0) || (cc != 0) || (dd != 0))
                    if((aa != 0) && (bb != 0) && (cc != 0) && (dd != 0))
                    {

                        //first row
                        matrix[0][0] = a;
                        matrix[0][1] = b;
                        matrix[0][2] = c;
                        matrix[0][3] = d;
                        //second row
                        matrix[1][0] = b;
                        matrix[1][1] = c;
                        matrix[1][2] = d;
                        matrix[1][3] = a;
                        //first row
                        matrix[2][0] = c;
                        matrix[2][1] = d;
                        matrix[2][2] = a;
                        matrix[2][3] = b;
                        //second row
                        matrix[3][0] = d;
                        matrix[3][1] = a;
                        matrix[3][2] = b;
                        matrix[3][3] = c;

                        totMatrix = totMatrix + 1;

                        det = determ(matrix, 4);

                        if(det != 0x00)
                        {

                            det = checkMDS2(matrix);

                            if(det != 0x00)
                            {
                                det = checkMDS3(matrix);

                                if(det != 0x00)
                                {
                                    invMatrix = invMatrix + 1;

                                    ec = equalCoeff(a, b, c, d);

                                    if(ec == 1)
                                    {
                                        eqCoef = eqCoef + 1;
                                        zeroSum = zeroSum + 1;
                                    }
                                    else
                                    {
                                        zeroSum = zeroSum + sumCoeffZero(a, b, c, d);
                                    }
                                }
                            }
                        }

                    }
                }
            }
        }
    }

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**This program counts the number of 4*4 Circulant Matrices with word of 8 bits with the following properties:

1)  - Invertible
    - Invertible and two equal coefficients in each row
    - Invertible and XoR-sum of >=2 coefficients equal to zero in each row

2)  - MDS
    - MDS and two equal coefficients in each row
    - MDS and XoR-sum of >=2 coefficients equal to zero in each row
*/

int main()
{
    FILE *fp;

    fp = fopen("Matrix_8bit.txt","w+");

    if(MDS == 0)
    {
        testMatrix();
    }
    else
    {
        testMatrixMDS();
    }

    if(MDS == 0)
        fprintf(fp, "Invertible: %f\n - Invertible and two equal coefficients in each row: %f\n - Invertible and XoR-sum of >=2 coefficients equal to zero in each row: %f\n\n", invMatrix, eqCoef, zeroSum);
    else
        fprintf(fp, "MDS: %f\n - MDS and two equal coefficients in each row: %f\n - MDS and XoR-sum of >=2 coefficients equal to zero in each row: %f\n\n", invMatrix, eqCoef, zeroSum);

    fclose(fp);

    return 0;
}
