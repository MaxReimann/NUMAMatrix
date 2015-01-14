#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sched.h>
#include <numa.h>

#include "globals.h"

#include "strassen.h"
#include "strassenutil.h"

#define _a11 a11->p[0]
#define _a12 a11->p[1]
#define _a13 a12->p[0]
#define _a14 a12->p[1]
#define _a21 a11->p[2]
#define _a22 a11->p[3]
#define _a23 a12->p[2]
#define _a24 a12->p[3]
#define _a31 a21->p[0]
#define _a32 a21->p[1]
#define _a33 a22->p[0]
#define _a34 a22->p[1]
#define _a41 a21->p[2]
#define _a42 a21->p[3]
#define _a43 a22->p[2]
#define _a44 a22->p[3]

#define _b11 b11->p[0]
#define _b12 b11->p[1]
#define _b13 b12->p[0]
#define _b14 b12->p[1]
#define _b21 b11->p[2]
#define _b22 b11->p[3]
#define _b23 b12->p[2]
#define _b24 b12->p[3]
#define _b31 b21->p[0]
#define _b32 b21->p[1]
#define _b33 b22->p[0]
#define _b34 b22->p[1]
#define _b41 b21->p[2]
#define _b42 b21->p[3]
#define _b43 b22->p[2]
#define _b44 b22->p[3]


#define C11 c11->p[0]
#define C12 c11->p[1]
#define C13 c12->p[0]
#define C14 c12->p[1]
#define C21 c11->p[2]
#define C22 c11->p[3]
#define C23 c12->p[2]
#define C24 c12->p[3]
#define C31 c21->p[0]
#define C32 c21->p[1]
#define C33 c22->p[0]
#define C34 c22->p[1]
#define C41 c21->p[2]
#define C42 c21->p[3]
#define C43 c22->p[2]
#define C44 c22->p[3]




#define _P11 (P[0]->p[3])
#define _P12 (P[1]->p[3])
#define _P13 (P[2]->p[3])
#define _P14 (P[3]->p[3])
#define _P15 (P[4]->p[3])
#define _P16 (P[5]->p[3])
#define _P17 (P[6]->p[3])

#define _P21 (P[7]->p[3])
#define _P22 (P[8]->p[3])
#define _P23 (P[9]->p[3])
#define _P24 (P[10]->p[3])
#define _P25 (P[11]->p[3])
#define _P26 (P[12]->p[3])
#define _P27 (P[13]->p[3])

#define _P31 (P[14]->p[3])
#define _P32 (P[15]->p[3])
#define _P33 (P[16]->p[3])
#define _P34 (P[17]->p[3])
#define _P35 (P[18]->p[3])
#define _P36 (P[19]->p[3])
#define _P37 (P[20]->p[3])

#define _P41 (P[21]->p[3])
#define _P42 (P[22]->p[3])
#define _P43 (P[23]->p[3])
#define _P44 (P[24]->p[3])
#define _P45 (P[25]->p[3])
#define _P46 (P[26]->p[3])
#define _P47 (P[27]->p[3])

#define _P51 (P[28]->p[3])
#define _P52 (P[29]->p[3])
#define _P53 (P[30]->p[3])
#define _P54 (P[31]->p[3])
#define _P55 (P[32]->p[3])
#define _P56 (P[33]->p[3])
#define _P57 (P[34]->p[3])

#define _P61 (P[35]->p[3])
#define _P62 (P[36]->p[3])
#define _P63 (P[37]->p[3])
#define _P64 (P[38]->p[3])
#define _P65 (P[39]->p[3])
#define _P66 (P[40]->p[3])
#define _P67 (P[41]->p[3])

#define _P71 (P[42]->p[3])
#define _P72 (P[43]->p[3])
#define _P73 (P[44]->p[3])
#define _P74 (P[45]->p[3])
#define _P75 (P[46]->p[3])
#define _P76 (P[47]->p[3])
#define _P77 (P[48]->p[3])

#define BILLION 1E9

/* template for function generation, do not modify
#start_definition
P11 = (a11 + a33 + a22 + a44)*(b11 + b33 + b22 + b44)
P12 = (a21 + a43 + a22 + a44)*(b11 + b33)
P13 = (a11 + a33)*(b12 + b34 - b22 - b44)
P14 = (a22 + a44)*(b21 + b43 - b11 - b33)
P15 = (a11 + a33 + a12 + a34)*(b22 + b44)
P16 = (a21 + a43 - a11 - a33)*(b11 + b33 + b12 + b34)
P17 = (a12 + a34 - a22 - a44)*(b21 + b43 + b22 + b44)
P21 = (a31 + a33 + a42 + a44)*(b11 + b22)
P22 = (a41 + a43 + a42 + a44)*b11
P23 = (a31 + a33)*(b12- b22)
P24 = (a42 + a44)*(b21 - b11)
P25 = (a31 + a33 + a32 + a34)*b22
P26 = (a41 + a43 - a31 - a33)*(b11 + b12)
P27 = (a32 + a34 - a42 - a44)*(b21 + b22)
P31 = (a11 + a22)*(b13 - b33 + b24 - b44)
P32 = (a21 + a22)*(b13 - b33)
P33 = a11*(b14 - b34 - b24 + b44)
P34 = a22*(b23 - b43 - b13 + b33)
P35 = (a11 + a12)*(b24 - b44)
P36 = (a21 - a11)*(b13 - b33 + b14 - b34)
P37 = (a12 - a22)*(b23 - b43 + b24 - b44)
P41 = (a33 + a44)*(b31 - b11 + b42 - b22)
P42 = (a43 + a44)*(b31 - b11)
P43 = a33*(b32 - b12 - b42 + b22)
P44 = a44*(b41 - b21 - b31 + b11)
P45 = (a33 + a34)*(b42 - b22)
P46 = (a43 - a33)*(b31 - b11 + b32 - b12)
P47 = (a34 - a44)*(b41 - b21 + b42 - b22)
P51 = (a11 + a13 + a22 + a24)*(b33 + b44)
P52 = (a21 + a23 + a22 + a24)*b33
P53 = (a11 + a13)*(b34 - b44)
P54 = (a22 + a24)*(b43 - b33)
P55 = (a11 + a13 + a12 + a14)*b44
P56 = (a21 + a23 - a11 - a13)*(b33 + b34)
P57 = (a12 + a14 - a22 - a24)*(b43 + b44)
P61 = (a31 - a11 + a42 - a22)*(b11 + b13 + b22 + b24)
P62 = (a41 - a21 + a42 - a22)*(b11 + b13)
P63 = (a31 - a11)*(b12 + b14 - b22 - b24)
P64 = (a42 - a22)*(b21 + b23 - b11 - b13)
P65 = (a31 - a11 + a32 - a12)*(b22 + b24)
P66 = (a41 - a21 - a31 + a11)*(b11 + b13 + b12 + b14)
P67 = (a32 - a12 - a42 + a22)*(b21 + b23 + b22 + b24)
P71 = (a13 - a33 + a24 - a44)*(b31 + b33 + b42 + b44)
P72 = (a23 - a43 + a24 - a44)*(b31 + b33)
P73 = (a13 - a33)*(b32 + b34 - b42 - b44)
P74 = (a24 - a44)*(b41 + b43 - b31 - b33)
P75 = (a13 - a33 + a14 - a34)*(b42 + b44)
P76 = (a23 - a43 - a13 + a33)*(b31 + b33 + b32 + b34)
P77 = (a14 - a34 - a24 + a44)*(b41 + b43 + b42 + b44)
#end_definition
*/
/*
#start_result_definition
c11 =  (P11 + P14 - P15 + P17) + (P41 + P44 - P45 + P47) - (P51 + P54 - P55 + P57) + (P71 + P74 - P75 + P77)
c12 =  (P13 + P15) + (P43 + P45) - (P53 + P55) + (P73 + P75)
c13 =  (P31 + P34 - P35 + P37) + (P51 + P54 - P55 + P57)
c14 =  (P33 + P35) + (P53 + P55)
c21 =  (P12 + P14) + (P42 + P44) - (P52 + P54) + (P72 + P74)
c22 =  (P11 + P13 - P12 + P16) + (P41 + P43 - P42 + P46) - (P51 + P53 - P52 + P56) + (P71 + P73 - P72 + P76)
c23 =  (P32 + P34) + (P52 + P54)
c24 =  (P31 + P33 - P32 + P36) + (P51 + P53 - P52 + P56)
c31 =  (P21 + P24 - P25 + P27) + (P41 + P44 - P45 + P47)
c32 =  (P23 + P25) + (P43 + P45)
c33 =  (P11 + P14 - P15 + P17) - (P21 + P24 - P25 + P27) + (P31 + P34 - P35 + P37) + (P61 + P64 - P65 + P67)
c34 =  (P13 + P15) - (P23 + P25) + (P33 + P35) + (P63 + P65)
c41 =  (P22 + P24) + (P42 + P44)
c42 =  (P21 + P23 - P22 + P26) + (P41 + P43 - P42 + P46)
c43 =  (P12 + P14) - (P22 + P24) + (P32 + P34) + (P62 + P64)
c44 =  (P11 + P13 - P12 + P16) - (P21 + P23 - P22 + P26) + (P31 + P33 - P32 + P36) + (P61 + P63 - P62 + P66)
#start_result_definition
*/

//////////////////// Strassen Parts /////////////////////
void P11(int n, matrix a, matrix b, matrix p)
{
add(n, _a11, p11, p11);
add(n, p11, _a33, p11);
add(n, p11, _a22, p11);
add(n, p11, _a44, p11);
add(n, _b11, p12, p12);
add(n, p12, _b33, p12);
add(n, p12, _b22, p12);
add(n, p12, _b44, p12);
strassen_multiply(n, p11, p12, p_res, p_temp);
}


void P12(int n, matrix a, matrix b, matrix p)
{
add(n, _a21, p11, p11);
add(n, p11, _a43, p11);
add(n, p11, _a22, p11);
add(n, p11, _a44, p11);
add(n, _b11, p12, p12);
add(n, p12, _b33, p12);
strassen_multiply(n, p11, p12, p_res, p_temp);
}


void P13(int n, matrix a, matrix b, matrix p)
{
add(n, _a11, p11, p11);
add(n, p11, _a33, p11);
add(n, _b12, p12, p12);
add(n, p12, _b34, p12);
sub(n, p12, _b22, p12);
sub(n, p12, _b44, p12);
strassen_multiply(n, p11, p12, p_res, p_temp);
}


void P14(int n, matrix a, matrix b, matrix p)
{
add(n, _a22, p11, p11);
add(n, p11, _a44, p11);
add(n, _b21, p12, p12);
add(n, p12, _b43, p12);
sub(n, p12, _b11, p12);
sub(n, p12, _b33, p12);
strassen_multiply(n, p11, p12, p_res, p_temp);
}


void P15(int n, matrix a, matrix b, matrix p)
{
add(n, _a11, p11, p11);
add(n, p11, _a33, p11);
add(n, p11, _a12, p11);
add(n, p11, _a34, p11);
add(n, _b22, p12, p12);
add(n, p12, _b44, p12);
strassen_multiply(n, p11, p12, p_res, p_temp);
}


void P16(int n, matrix a, matrix b, matrix p)
{
add(n, _a21, p11, p11);
add(n, p11, _a43, p11);
sub(n, p11, _a11, p11);
sub(n, p11, _a33, p11);
add(n, _b11, p12, p12);
add(n, p12, _b33, p12);
add(n, p12, _b12, p12);
add(n, p12, _b34, p12);
strassen_multiply(n, p11, p12, p_res, p_temp);
}


void P17(int n, matrix a, matrix b, matrix p)
{
add(n, _a12, p11, p11);
add(n, p11, _a34, p11);
sub(n, p11, _a22, p11);
sub(n, p11, _a44, p11);
add(n, _b21, p12, p12);
add(n, p12, _b43, p12);
add(n, p12, _b22, p12);
add(n, p12, _b44, p12);
strassen_multiply(n, p11, p12, p_res, p_temp);
}


void P21(int n, matrix a, matrix b, matrix p)
{
add(n, _a31, p11, p11);
add(n, p11, _a33, p11);
add(n, p11, _a42, p11);
add(n, p11, _a44, p11);
add(n, _b11, p12, p12);
add(n, p12, _b22, p12);
strassen_multiply(n, p11, p12, p_res, p_temp);
}


void P22(int n, matrix a, matrix b, matrix p)
{
add(n, _a41, p11, p11);
add(n, p11, _a43, p11);
add(n, p11, _a42, p11);
add(n, p11, _a44, p11);
strassen_multiply(n, p11, _b11, p_res, p_temp);
}


void P23(int n, matrix a, matrix b, matrix p)
{
add(n, _a31, p11, p11);
add(n, p11, _a33, p11);
sub(n, _b12, p12, p12);
sub(n, p12, _b22, p12);
strassen_multiply(n, p11, p12, p_res, p_temp);
}


void P24(int n, matrix a, matrix b, matrix p)
{
add(n, _a42, p11, p11);
add(n, p11, _a44, p11);
sub(n, _b21, p12, p12);
sub(n, p12, _b11, p12);
strassen_multiply(n, p11, p12, p_res, p_temp);
}


void P25(int n, matrix a, matrix b, matrix p)
{
add(n, _a31, p11, p11);
add(n, p11, _a33, p11);
add(n, p11, _a32, p11);
add(n, p11, _a34, p11);
strassen_multiply(n, p11, _b22, p_res, p_temp);
}


void P26(int n, matrix a, matrix b, matrix p)
{
add(n, _a41, p11, p11);
add(n, p11, _a43, p11);
sub(n, p11, _a31, p11);
sub(n, p11, _a33, p11);
add(n, _b11, p12, p12);
add(n, p12, _b12, p12);
strassen_multiply(n, p11, p12, p_res, p_temp);
}


void P27(int n, matrix a, matrix b, matrix p)
{
add(n, _a32, p11, p11);
add(n, p11, _a34, p11);
sub(n, p11, _a42, p11);
sub(n, p11, _a44, p11);
add(n, _b21, p12, p12);
add(n, p12, _b22, p12);
strassen_multiply(n, p11, p12, p_res, p_temp);
}


void P31(int n, matrix a, matrix b, matrix p)
{
add(n, _a11, p11, p11);
add(n, p11, _a22, p11);
sub(n, _b13, p12, p12);
sub(n, p12, _b33, p12);
add(n, p12, _b24, p12);
sub(n, p12, _b44, p12);
strassen_multiply(n, p11, p12, p_res, p_temp);
}


void P32(int n, matrix a, matrix b, matrix p)
{
add(n, _a21, p11, p11);
add(n, p11, _a22, p11);
sub(n, _b13, p12, p12);
sub(n, p12, _b33, p12);
strassen_multiply(n, p11, p12, p_res, p_temp);
}


void P33(int n, matrix a, matrix b, matrix p)
{
sub(n, _b14, p12, p12);
sub(n, p12, _b34, p12);
sub(n, p12, _b24, p12);
add(n, p12, _b44, p12);
strassen_multiply(n, _a11, p12, p_res, p_temp);
}


void P34(int n, matrix a, matrix b, matrix p)
{
sub(n, _b23, p12, p12);
sub(n, p12, _b43, p12);
sub(n, p12, _b13, p12);
add(n, p12, _b33, p12);
strassen_multiply(n, _a22, p12, p_res, p_temp);
}


void P35(int n, matrix a, matrix b, matrix p)
{
add(n, _a11, p11, p11);
add(n, p11, _a12, p11);
sub(n, _b24, p12, p12);
sub(n, p12, _b44, p12);
strassen_multiply(n, p11, p12, p_res, p_temp);
}


void P36(int n, matrix a, matrix b, matrix p)
{
sub(n, _a21, p11, p11);
sub(n, p11, _a11, p11);
sub(n, _b13, p12, p12);
sub(n, p12, _b33, p12);
add(n, p12, _b14, p12);
sub(n, p12, _b34, p12);
strassen_multiply(n, p11, p12, p_res, p_temp);
}


void P37(int n, matrix a, matrix b, matrix p)
{
sub(n, _a12, p11, p11);
sub(n, p11, _a22, p11);
sub(n, _b23, p12, p12);
sub(n, p12, _b43, p12);
add(n, p12, _b24, p12);
sub(n, p12, _b44, p12);
strassen_multiply(n, p11, p12, p_res, p_temp);
}


void P41(int n, matrix a, matrix b, matrix p)
{
add(n, _a33, p11, p11);
add(n, p11, _a44, p11);
sub(n, _b31, p12, p12);
sub(n, p12, _b11, p12);
add(n, p12, _b42, p12);
sub(n, p12, _b22, p12);
strassen_multiply(n, p11, p12, p_res, p_temp);
}


void P42(int n, matrix a, matrix b, matrix p)
{
add(n, _a43, p11, p11);
add(n, p11, _a44, p11);
sub(n, _b31, p12, p12);
sub(n, p12, _b11, p12);
strassen_multiply(n, p11, p12, p_res, p_temp);
}


void P43(int n, matrix a, matrix b, matrix p)
{
sub(n, _b32, p12, p12);
sub(n, p12, _b12, p12);
sub(n, p12, _b42, p12);
add(n, p12, _b22, p12);
strassen_multiply(n, _a33, p12, p_res, p_temp);
}


void P44(int n, matrix a, matrix b, matrix p)
{
sub(n, _b41, p12, p12);
sub(n, p12, _b21, p12);
sub(n, p12, _b31, p12);
add(n, p12, _b11, p12);
strassen_multiply(n, _a44, p12, p_res, p_temp);
}


void P45(int n, matrix a, matrix b, matrix p)
{
add(n, _a33, p11, p11);
add(n, p11, _a34, p11);
sub(n, _b42, p12, p12);
sub(n, p12, _b22, p12);
strassen_multiply(n, p11, p12, p_res, p_temp);
}


void P46(int n, matrix a, matrix b, matrix p)
{
sub(n, _a43, p11, p11);
sub(n, p11, _a33, p11);
sub(n, _b31, p12, p12);
sub(n, p12, _b11, p12);
add(n, p12, _b32, p12);
sub(n, p12, _b12, p12);
strassen_multiply(n, p11, p12, p_res, p_temp);
}


void P47(int n, matrix a, matrix b, matrix p)
{
sub(n, _a34, p11, p11);
sub(n, p11, _a44, p11);
sub(n, _b41, p12, p12);
sub(n, p12, _b21, p12);
add(n, p12, _b42, p12);
sub(n, p12, _b22, p12);
strassen_multiply(n, p11, p12, p_res, p_temp);
}


void P51(int n, matrix a, matrix b, matrix p)
{
add(n, _a11, p11, p11);
add(n, p11, _a13, p11);
add(n, p11, _a22, p11);
add(n, p11, _a24, p11);
add(n, _b33, p12, p12);
add(n, p12, _b44, p12);
strassen_multiply(n, p11, p12, p_res, p_temp);
}


void P52(int n, matrix a, matrix b, matrix p)
{
add(n, _a21, p11, p11);
add(n, p11, _a23, p11);
add(n, p11, _a22, p11);
add(n, p11, _a24, p11);
strassen_multiply(n, p11, _b33, p_res, p_temp);
}


void P53(int n, matrix a, matrix b, matrix p)
{
add(n, _a11, p11, p11);
add(n, p11, _a13, p11);
sub(n, _b34, p12, p12);
sub(n, p12, _b44, p12);
strassen_multiply(n, p11, p12, p_res, p_temp);
}


void P54(int n, matrix a, matrix b, matrix p)
{
add(n, _a22, p11, p11);
add(n, p11, _a24, p11);
sub(n, _b43, p12, p12);
sub(n, p12, _b33, p12);
strassen_multiply(n, p11, p12, p_res, p_temp);
}


void P55(int n, matrix a, matrix b, matrix p)
{
add(n, _a11, p11, p11);
add(n, p11, _a13, p11);
add(n, p11, _a12, p11);
add(n, p11, _a14, p11);
strassen_multiply(n, p11, _b44, p_res, p_temp);
}


void P56(int n, matrix a, matrix b, matrix p)
{
add(n, _a21, p11, p11);
add(n, p11, _a23, p11);
sub(n, p11, _a11, p11);
sub(n, p11, _a13, p11);
add(n, _b33, p12, p12);
add(n, p12, _b34, p12);
strassen_multiply(n, p11, p12, p_res, p_temp);
}


void P57(int n, matrix a, matrix b, matrix p)
{
add(n, _a12, p11, p11);
add(n, p11, _a14, p11);
sub(n, p11, _a22, p11);
sub(n, p11, _a24, p11);
add(n, _b43, p12, p12);
add(n, p12, _b44, p12);
strassen_multiply(n, p11, p12, p_res, p_temp);
}


void P61(int n, matrix a, matrix b, matrix p)
{
sub(n, _a31, p11, p11);
sub(n, p11, _a11, p11);
add(n, p11, _a42, p11);
sub(n, p11, _a22, p11);
add(n, _b11, p12, p12);
add(n, p12, _b13, p12);
add(n, p12, _b22, p12);
add(n, p12, _b24, p12);
strassen_multiply(n, p11, p12, p_res, p_temp);
}


void P62(int n, matrix a, matrix b, matrix p)
{
sub(n, _a41, p11, p11);
sub(n, p11, _a21, p11);
add(n, p11, _a42, p11);
sub(n, p11, _a22, p11);
add(n, _b11, p12, p12);
add(n, p12, _b13, p12);
strassen_multiply(n, p11, p12, p_res, p_temp);
}


void P63(int n, matrix a, matrix b, matrix p)
{
sub(n, _a31, p11, p11);
sub(n, p11, _a11, p11);
add(n, _b12, p12, p12);
add(n, p12, _b14, p12);
sub(n, p12, _b22, p12);
sub(n, p12, _b24, p12);
strassen_multiply(n, p11, p12, p_res, p_temp);
}


void P64(int n, matrix a, matrix b, matrix p)
{
sub(n, _a42, p11, p11);
sub(n, p11, _a22, p11);
add(n, _b21, p12, p12);
add(n, p12, _b23, p12);
sub(n, p12, _b11, p12);
sub(n, p12, _b13, p12);
strassen_multiply(n, p11, p12, p_res, p_temp);
}


void P65(int n, matrix a, matrix b, matrix p)
{
sub(n, _a31, p11, p11);
sub(n, p11, _a11, p11);
add(n, p11, _a32, p11);
sub(n, p11, _a12, p11);
add(n, _b22, p12, p12);
add(n, p12, _b24, p12);
strassen_multiply(n, p11, p12, p_res, p_temp);
}


void P66(int n, matrix a, matrix b, matrix p)
{
sub(n, _a41, p11, p11);
sub(n, p11, _a21, p11);
sub(n, p11, _a31, p11);
add(n, p11, _a11, p11);
add(n, _b11, p12, p12);
add(n, p12, _b13, p12);
add(n, p12, _b12, p12);
add(n, p12, _b14, p12);
strassen_multiply(n, p11, p12, p_res, p_temp);
}


void P67(int n, matrix a, matrix b, matrix p)
{
sub(n, _a32, p11, p11);
sub(n, p11, _a12, p11);
sub(n, p11, _a42, p11);
add(n, p11, _a22, p11);
add(n, _b21, p12, p12);
add(n, p12, _b23, p12);
add(n, p12, _b22, p12);
add(n, p12, _b24, p12);
strassen_multiply(n, p11, p12, p_res, p_temp);
}


void P71(int n, matrix a, matrix b, matrix p)
{
sub(n, _a13, p11, p11);
sub(n, p11, _a33, p11);
add(n, p11, _a24, p11);
sub(n, p11, _a44, p11);
add(n, _b31, p12, p12);
add(n, p12, _b33, p12);
add(n, p12, _b42, p12);
add(n, p12, _b44, p12);
strassen_multiply(n, p11, p12, p_res, p_temp);
}


void P72(int n, matrix a, matrix b, matrix p)
{
sub(n, _a23, p11, p11);
sub(n, p11, _a43, p11);
add(n, p11, _a24, p11);
sub(n, p11, _a44, p11);
add(n, _b31, p12, p12);
add(n, p12, _b33, p12);
strassen_multiply(n, p11, p12, p_res, p_temp);
}


void P73(int n, matrix a, matrix b, matrix p)
{
sub(n, _a13, p11, p11);
sub(n, p11, _a33, p11);
add(n, _b32, p12, p12);
add(n, p12, _b34, p12);
sub(n, p12, _b42, p12);
sub(n, p12, _b44, p12);
strassen_multiply(n, p11, p12, p_res, p_temp);
}


void P74(int n, matrix a, matrix b, matrix p)
{
sub(n, _a24, p11, p11);
sub(n, p11, _a44, p11);
add(n, _b41, p12, p12);
add(n, p12, _b43, p12);
sub(n, p12, _b31, p12);
sub(n, p12, _b33, p12);
strassen_multiply(n, p11, p12, p_res, p_temp);
}


void P75(int n, matrix a, matrix b, matrix p)
{
sub(n, _a13, p11, p11);
sub(n, p11, _a33, p11);
add(n, p11, _a14, p11);
sub(n, p11, _a34, p11);
add(n, _b42, p12, p12);
add(n, p12, _b44, p12);
strassen_multiply(n, p11, p12, p_res, p_temp);
}


void P76(int n, matrix a, matrix b, matrix p)
{
sub(n, _a23, p11, p11);
sub(n, p11, _a43, p11);
sub(n, p11, _a13, p11);
add(n, p11, _a33, p11);
add(n, _b31, p12, p12);
add(n, p12, _b33, p12);
add(n, p12, _b32, p12);
add(n, p12, _b34, p12);
strassen_multiply(n, p11, p12, p_res, p_temp);
}


void P77(int n, matrix a, matrix b, matrix p)
{
sub(n, _a14, p11, p11);
sub(n, p11, _a34, p11);
sub(n, p11, _a24, p11);
add(n, p11, _a44, p11);
add(n, _b41, p12, p12);
add(n, p12, _b43, p12);
add(n, p12, _b42, p12);
add(n, p12, _b44, p12);
strassen_multiply(n, p11, p12, p_res, p_temp);
}


void _c11(int n, matrix P[], matrix _c11)
{
matrix temp = strassen_newmatrix(n);
add(n, _P11, _c11, _c11);
add(n, _c11, _P14, _c11);
sub(n, _c11, _P15, _c11);
add(n, _c11, _P17, _c11);
add(n, _P41, _c11, _c11);
add(n, _c11, _P44, _c11);
sub(n, _c11, _P45, _c11);
add(n, _c11, _P47, _c11);
add(n, _P51, temp, temp);
add(n, temp, _P54, temp);
sub(n, temp, _P55, temp);
add(n, temp, _P57, temp);
sub(n, _c11, temp, _c11);
add(n, _P71, _c11, _c11);
add(n, _c11, _P74, _c11);
sub(n, _c11, _P75, _c11);
add(n, _c11, _P77, _c11);
}


void _c12(int n, matrix P[], matrix _c12)
{
matrix temp = strassen_newmatrix(n);
add(n, _P13, _P15, _c12);
add(n, _P43, _c12, _c12);
add(n, _c12, _P45, _c12);
add(n, _P53, temp, temp);
add(n, temp, _P55, temp);
sub(n, _c12, temp, _c12);
add(n, _P73, _c12, _c12);
add(n, _c12, _P75, _c12);
}


void _c13(int n, matrix P[], matrix _c13)
{
add(n, _P31, _c13, _c13);
add(n, _c13, _P34, _c13);
sub(n, _c13, _P35, _c13);
add(n, _c13, _P37, _c13);
add(n, _P51, _c13, _c13);
add(n, _c13, _P54, _c13);
sub(n, _c13, _P55, _c13);
add(n, _c13, _P57, _c13);
}


void _c14(int n, matrix P[], matrix _c14)
{
add(n, _P33, _P35, _c14);
add(n, _P53, _c14, _c14);
add(n, _c14, _P55, _c14);
}


void _c21(int n, matrix P[], matrix _c21)
{
matrix temp = strassen_newmatrix(n);
add(n, _P12, _P14, _c21);
add(n, _P42, _c21, _c21);
add(n, _c21, _P44, _c21);
add(n, _P52, temp, temp);
add(n, temp, _P54, temp);
sub(n, _c21, temp, _c21);
add(n, _P72, _c21, _c21);
add(n, _c21, _P74, _c21);
}


void _c22(int n, matrix P[], matrix _c22)
{
matrix temp = strassen_newmatrix(n);
add(n, _P11, _c22, _c22);
add(n, _c22, _P13, _c22);
sub(n, _c22, _P12, _c22);
add(n, _c22, _P16, _c22);
add(n, _P41, _c22, _c22);
add(n, _c22, _P43, _c22);
sub(n, _c22, _P42, _c22);
add(n, _c22, _P46, _c22);
add(n, _P51, temp, temp);
add(n, temp, _P53, temp);
sub(n, temp, _P52, temp);
add(n, temp, _P56, temp);
sub(n, _c22, temp, _c22);
add(n, _P71, _c22, _c22);
add(n, _c22, _P73, _c22);
sub(n, _c22, _P72, _c22);
add(n, _c22, _P76, _c22);
}


void _c23(int n, matrix P[], matrix _c23)
{
add(n, _P32, _P34, _c23);
add(n, _P52, _c23, _c23);
add(n, _c23, _P54, _c23);
}


void _c24(int n, matrix P[], matrix _c24)
{
add(n, _P31, _c24, _c24);
add(n, _c24, _P33, _c24);
sub(n, _c24, _P32, _c24);
add(n, _c24, _P36, _c24);
add(n, _P51, _c24, _c24);
add(n, _c24, _P53, _c24);
sub(n, _c24, _P52, _c24);
add(n, _c24, _P56, _c24);
}


void _c31(int n, matrix P[], matrix _c31)
{
add(n, _P21, _c31, _c31);
add(n, _c31, _P24, _c31);
sub(n, _c31, _P25, _c31);
add(n, _c31, _P27, _c31);
add(n, _P41, _c31, _c31);
add(n, _c31, _P44, _c31);
sub(n, _c31, _P45, _c31);
add(n, _c31, _P47, _c31);
}


void _c32(int n, matrix P[], matrix _c32)
{
add(n, _P23, _P25, _c32);
add(n, _P43, _c32, _c32);
add(n, _c32, _P45, _c32);
}


void _c33(int n, matrix P[], matrix _c33)
{
matrix temp = strassen_newmatrix(n);
add(n, _P11, _c33, _c33);
add(n, _c33, _P14, _c33);
sub(n, _c33, _P15, _c33);
add(n, _c33, _P17, _c33);
add(n, _P21, temp, temp);
add(n, temp, _P24, temp);
sub(n, temp, _P25, temp);
add(n, temp, _P27, temp);
sub(n, _c33, temp, _c33);
add(n, _P31, _c33, _c33);
add(n, _c33, _P34, _c33);
sub(n, _c33, _P35, _c33);
add(n, _c33, _P37, _c33);
add(n, _P61, _c33, _c33);
add(n, _c33, _P64, _c33);
sub(n, _c33, _P65, _c33);
add(n, _c33, _P67, _c33);
}


void _c34(int n, matrix P[], matrix _c34)
{
matrix temp = strassen_newmatrix(n);
add(n, _P13, _P15, _c34);
add(n, _P23, temp, temp);
add(n, temp, _P25, temp);
sub(n, _c34, temp, _c34);
add(n, _P33, _c34, _c34);
add(n, _c34, _P35, _c34);
add(n, _P63, _c34, _c34);
add(n, _c34, _P65, _c34);
}


void _c41(int n, matrix P[], matrix _c41)
{
add(n, _P22, _P24, _c41);
add(n, _P42, _c41, _c41);
add(n, _c41, _P44, _c41);
}


void _c42(int n, matrix P[], matrix _c42)
{
add(n, _P21, _c42, _c42);
add(n, _c42, _P23, _c42);
sub(n, _c42, _P22, _c42);
add(n, _c42, _P26, _c42);
add(n, _P41, _c42, _c42);
add(n, _c42, _P43, _c42);
sub(n, _c42, _P42, _c42);
add(n, _c42, _P46, _c42);
}


void _c43(int n, matrix P[], matrix _c43)
{
matrix temp = strassen_newmatrix(n);
add(n, _P12, _P14, _c43);
add(n, _P22, temp, temp);
add(n, temp, _P24, temp);
sub(n, _c43, temp, _c43);
add(n, _P32, _c43, _c43);
add(n, _c43, _P34, _c43);
add(n, _P62, _c43, _c43);
add(n, _c43, _P64, _c43);
}


void _c44(int n, matrix P[], matrix _c44)
{
matrix temp = strassen_newmatrix(n);
add(n, _P11, _c44, _c44);
add(n, _c44, _P13, _c44);
sub(n, _c44, _P12, _c44);
add(n, _c44, _P16, _c44);
add(n, _P21, temp, temp);
add(n, temp, _P23, temp);
sub(n, temp, _P22, temp);
add(n, temp, _P26, temp);
sub(n, _c44, temp, _c44);
add(n, _P31, _c44, _c44);
add(n, _c44, _P33, _c44);
sub(n, _c44, _P32, _c44);
add(n, _c44, _P36, _c44);
add(n, _P61, _c44, _c44);
add(n, _c44, _P63, _c44);
sub(n, _c44, _P62, _c44);
add(n, _c44, _P66, _c44);
}




///////////////////// Parallelization //////////////////

typedef enum { false, true } bool;

typedef struct
{
    int n;
    matrix a;
    matrix b;
    matrix output;
    void (*p_fPtr)(int, matrix, matrix, matrix);
    void (*c_fPtr)(int, matrix[], matrix);
    int index;
    int node;
    int usesNuma;
    matrix *P;

} threadArguments;


void (*p_fPtr[49]) (int, matrix, matrix, matrix);

void (*c_fPtr[49]) (int n, matrix P[], matrix);

void gen_initFunctionPointers(){
	
p_fPtr[0] = P11;
p_fPtr[1] = P12;
p_fPtr[2] = P13;
p_fPtr[3] = P14;
p_fPtr[4] = P15;
p_fPtr[5] = P16;
p_fPtr[6] = P17;
p_fPtr[7] = P21;
p_fPtr[8] = P22;
p_fPtr[9] = P23;
p_fPtr[10] = P24;
p_fPtr[11] = P25;
p_fPtr[12] = P26;
p_fPtr[13] = P27;
p_fPtr[14] = P31;
p_fPtr[15] = P32;
p_fPtr[16] = P33;
p_fPtr[17] = P34;
p_fPtr[18] = P35;
p_fPtr[19] = P36;
p_fPtr[20] = P37;
p_fPtr[21] = P41;
p_fPtr[22] = P42;
p_fPtr[23] = P43;
p_fPtr[24] = P44;
p_fPtr[25] = P45;
p_fPtr[26] = P46;
p_fPtr[27] = P47;
p_fPtr[28] = P51;
p_fPtr[29] = P52;
p_fPtr[30] = P53;
p_fPtr[31] = P54;
p_fPtr[32] = P55;
p_fPtr[33] = P56;
p_fPtr[34] = P57;
p_fPtr[35] = P61;
p_fPtr[36] = P62;
p_fPtr[37] = P63;
p_fPtr[38] = P64;
p_fPtr[39] = P65;
p_fPtr[40] = P66;
p_fPtr[41] = P67;
p_fPtr[42] = P71;
p_fPtr[43] = P72;
p_fPtr[44] = P73;
p_fPtr[45] = P74;
p_fPtr[46] = P75;
p_fPtr[47] = P76;
p_fPtr[48] = P77;
c_fPtr[0] = _c11;
c_fPtr[1] = _c12;
c_fPtr[2] = _c13;
c_fPtr[3] = _c14;
c_fPtr[4] = _c21;
c_fPtr[5] = _c22;
c_fPtr[6] = _c23;
c_fPtr[7] = _c24;
c_fPtr[8] = _c31;
c_fPtr[9] = _c32;
c_fPtr[10] = _c33;
c_fPtr[11] = _c34;
c_fPtr[12] = _c41;
c_fPtr[13] = _c42;
c_fPtr[14] = _c43;
c_fPtr[15] = _c44;

}


bool gen_runningThreads[49];


void *gen_parallelDispatcherP(void *args)
{
    threadArguments *a = (threadArguments *) args;
    a->p_fPtr(a->n, a->a, a->b, a->output);
    if (a->usesNuma)
    	numa_run_on_node(a->node);

    gen_runningThreads[a->index] = false;
    pthread_exit((void *) args);
}

void *gen_parallelDispatcherC(void *args)
{

    threadArguments *a = (threadArguments *) args;

    a->c_fPtr(a->n, a->P, a->output);

    gen_runningThreads[a->index] = false;
    pthread_exit((void *) args);
}

int gen_numRunning()
{
    int running = 0;
    for (int i = 0; i < 49; i++)
        if (gen_runningThreads[i] == true)
            running++;
    return running;
}

int gen_firstRunning()
{
    for (int i = 0; i < 49; i++)
        if (gen_runningThreads[i] == true)
            return i;
    return -1;
}

void gen_parallelExecuteParts(threadArguments parts[], int n_parts, void* (*dispatcher)(void*))
{

    pthread_attr_t attr;
    void *status;
    pthread_t thread[n_parts];
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);


    for (int i = 0; i < n_parts; i++)
    {
        if (gen_numRunning() >= NUM_THREADS)
        {
            //wait until one is free
            int rc = pthread_join(thread[gen_firstRunning()], &status);
            if (rc)
            {
                fprintf(stderr, "%s: %s\n", "ERROR; return code from pthread_join()(1) ", strerror(rc));
                exit(EXIT_FAILURE);
            }
        }

        parts[i].index = i;
        int rc = pthread_create( &thread[i],  &attr, dispatcher, (void *) &parts[i]);
        if (rc)
        {
            fprintf(stderr, "Error - pthread_create() return code: %d\n", rc);
            exit(EXIT_FAILURE);
        }
        gen_runningThreads[i] = true;
    }


    /* Free attribute and wait for the other threads */
    pthread_attr_destroy(&attr);
    for (int i = 0; i < n_parts; i++)
    {
        if (gen_runningThreads[i]==true)
        {
            int rc = pthread_join(thread[i], &status);
            if (rc)
            {
                fprintf(stderr, "%s: %s\n", "ERROR; return code from pthread_join()(2) ", strerror(rc));
                exit(EXIT_FAILURE);
            }
        }
    }


}

void strassenMassiveParallel(int n, double first[], double second[], double multiply[])
{
    printf("Running parallel strassenMultiplication\n");
    matrix a, b, c;
    threadArguments parts[49];
    threadArguments partsC[16];
    matrix P[49];

    struct timespec start, end, totalStart, totalEnd;
    clock_gettime(CLOCK_MONOTONIC, &start);
    clock_gettime(CLOCK_MONOTONIC, &totalStart);

    gen_initFunctionPointers();

    int size_matrix = sizeofMatrix(n);
 
    a = strassen_newmatrix_block(n);
    b = strassen_newmatrix_block(n);
    c = strassen_newmatrix_block(n);

    strassen_set(n, a, first, 0 , 0);
    strassen_set(n, b, second, 0, 0);

    clock_gettime(CLOCK_MONOTONIC, &end);
    float seconds = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / BILLION;

    printf("preperation Time: %f\n\n", seconds);


    //struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);


    n /= 2;
    

        for (int i = 0; i<49; i++)
        {
            P[i] = strassen_newmatrix_block(n);
            parts[i].n = n/2;
            parts[i].a = a;
            parts[i].b = b;
            parts[i].output = P[i];
            parts[i].p_fPtr = p_fPtr[i];
            parts[i].index = i;
            parts[i].usesNuma = 1;

            gen_runningThreads[i] = false;
        }


    //blocks until all parts are executed
    gen_parallelExecuteParts(parts, 49, gen_parallelDispatcherP);

    clock_gettime(CLOCK_MONOTONIC, &end);
    seconds = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / BILLION;

    printf("multiplication took: %f\n\n", seconds);
    
    clock_gettime(CLOCK_MONOTONIC, &start);

    matrix result_submatrix[16] = {C11,C12,C13,C14,
        C21,C22,C23,C24,
        C31,C32,C33,C34,
        C41,C42,C43,C44};


    for (int i=0; i<16;i++)
    {
        partsC[i].index = i;
        partsC[i].n = n/2;
        partsC[i].P = P;
        partsC[i].output = result_submatrix[i];
        partsC[i].c_fPtr = c_fPtr[i];
        gen_runningThreads[i] = false;
    }


    //blocks until all parts are executed
    gen_parallelExecuteParts(partsC, 16, gen_parallelDispatcherC);

    strassen_get(n * 2, c, multiply, 0, 0);


    clock_gettime(CLOCK_MONOTONIC, &end);
    seconds = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / BILLION;

    printf("end part took: %f\n\n", seconds);

    clock_gettime(CLOCK_MONOTONIC, &totalEnd);

    seconds = (totalEnd.tv_sec - totalStart.tv_sec) + 
    	(totalEnd.tv_nsec - totalStart.tv_nsec) / BILLION;

    printf("total Time: %f\n\n", seconds);
}



void strassenMassiveParallelNUMA(int n, double first[], double second[], double multiply[])
{
    #define NUMA_NODES 8


    printf("Running parallel strassenMultiplication\n");
    matrix a[NUMA_NODES], b[NUMA_NODES], c;
    threadArguments parts[49];
    threadArguments partsC[16];
    matrix P[49];

    struct timespec start, end, totalStart, totalEnd;
    clock_gettime(CLOCK_MONOTONIC, &start);
    clock_gettime(CLOCK_MONOTONIC, &totalStart);

    gen_initFunctionPointers();

    int size_matrix = sizeofMatrix(n);
 
    a[0] = strassen_newmatrix_block(n);
    b[0] = strassen_newmatrix_block(n);
    c = strassen_newmatrix_block(n);

    strassen_set(n, a[0], first, 0 , 0);
    strassen_set(n, b[0], second, 0, 0);

    for (int i=1; i < NUMA_NODES; i++)
    {
    	a[i] = numa_alloc_onnode(size_matrix, i);
    	b[i] = numa_alloc_onnode(size_matrix, i);

        memcpy(a[i], a[0], size_matrix);
        memcpy(b[i], b[0], size_matrix);

        //numa_tonode_memory((void*) a[i], size_matrix, i);
        //numa_tonode_memory((void*) b[i], size_matrix, i);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    float seconds = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / BILLION;

    printf("preperation Time: %f\n\n", seconds);


    //struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);


    n /= 2;
    
    for (int node=0; node < NUMA_NODES;node++)
    {
        int start = node * 49/NUMA_NODES;
        int end = (node+1) * 49/NUMA_NODES -1;
        for (int i = start; i<=end; i++)
        {
            P[i] = strassen_newmatrix_block(n);
            parts[i].n = n/2;
            parts[i].a = a[node];
            parts[i].b = b[node];
            parts[i].node = node;
            parts[i].output = P[i];
            parts[i].p_fPtr = p_fPtr[i];
            parts[i].index = i;
            parts[i].usesNuma = -1;

            gen_runningThreads[i] = false;
        }
        
    }


    //blocks until all parts are executed
    gen_parallelExecuteParts(parts, 49, gen_parallelDispatcherP);

    clock_gettime(CLOCK_MONOTONIC, &end);
    seconds = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / BILLION;

    printf("multiplication took: %f\n\n", seconds);
    
    clock_gettime(CLOCK_MONOTONIC, &start);

    matrix result_submatrix[16] = {C11,C12,C13,C14,
        C21,C22,C23,C24,
        C31,C32,C33,C34,
        C41,C42,C43,C44};


    for (int i=0; i<16;i++)
    {
        partsC[i].index = i;
        partsC[i].n = n/2;
        partsC[i].P = P;
        partsC[i].output = result_submatrix[i];
        partsC[i].c_fPtr = c_fPtr[i];
        gen_runningThreads[i] = false;
    }


    //blocks until all parts are executed
    gen_parallelExecuteParts(partsC, 16, gen_parallelDispatcherC);

    strassen_get(n * 2, c, multiply, 0, 0);


    clock_gettime(CLOCK_MONOTONIC, &end);
    seconds = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / BILLION;

    printf("end part took: %f\n\n", seconds);

    clock_gettime(CLOCK_MONOTONIC, &totalEnd);

    seconds = (totalEnd.tv_sec - totalStart.tv_sec) + 
    	(totalEnd.tv_nsec - totalStart.tv_nsec) / BILLION;

    printf("total Time: %f\n\n", seconds);
}