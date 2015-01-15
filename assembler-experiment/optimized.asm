GAS LISTING /tmp/ccxAlghv.s 			page 1


   1              		.file	"simpleMMM.c"
   2              		.text
   3              	.Ltext0:
   4              		.section	.rodata.str1.1,"aMS",@progbits,1
   5              	.LC2:
   6 0000 626C6F63 		.string	"blocked took: %f\n\n"
   6      6B656420 
   6      746F6F6B 
   6      3A202566 
   6      0A0A00
   7              		.text
   8              		.p2align 4,,15
   9              		.globl	blockedMultiply
  11              	blockedMultiply:
  12              	.LFB545:
  13              		.file 1 "simpleMMM.c"
   1:simpleMMM.c   **** #include <xmmintrin.h>
   2:simpleMMM.c   **** #include <stdio.h>
   3:simpleMMM.c   **** #include <time.h>
   4:simpleMMM.c   **** #include <time.h>
   5:simpleMMM.c   **** 
   6:simpleMMM.c   **** #include <stdlib.h>
   7:simpleMMM.c   **** #define BILLION 1E9
   8:simpleMMM.c   **** #define IDX(Y, X) (ndim * Y + X) //rows first
   9:simpleMMM.c   **** 
  10:simpleMMM.c   **** const int ndim = 2000;
  11:simpleMMM.c   **** 
  12:simpleMMM.c   **** void blockedMultiply(double A[], double B[], double C[]);
  13:simpleMMM.c   **** 
  14:simpleMMM.c   **** int main(int argc, char **argv)
  15:simpleMMM.c   **** {
  16:simpleMMM.c   **** 
  17:simpleMMM.c   **** 
  18:simpleMMM.c   ****     srand(1);
  19:simpleMMM.c   ****     double *first = malloc(ndim * ndim * sizeof(double));
  20:simpleMMM.c   ****     double *second = malloc(ndim * ndim * sizeof(double));
  21:simpleMMM.c   ****     double *multiply = malloc(ndim * ndim * sizeof(double));
  22:simpleMMM.c   ****     int i, c, d;
  23:simpleMMM.c   ****     printf("ndim: %d\n", ndim);
  24:simpleMMM.c   ****     setbuf(stdout, NULL);
  25:simpleMMM.c   **** 
  26:simpleMMM.c   **** 
  27:simpleMMM.c   ****     for (c = 0 ; c < ndim; c++)
  28:simpleMMM.c   ****     {
  29:simpleMMM.c   ****         for (d = 0 ; d < ndim; d++)
  30:simpleMMM.c   ****         {
  31:simpleMMM.c   ****             first[IDX(c, d)] = rand() % 10; //int between 0 - 1000
  32:simpleMMM.c   ****             second[IDX(c, d)] = rand() % 10;
  33:simpleMMM.c   ****             multiply[IDX(c, d)] = 0;
  34:simpleMMM.c   **** 
  35:simpleMMM.c   ****         }
  36:simpleMMM.c   ****     }
  37:simpleMMM.c   **** 
  38:simpleMMM.c   ****     blockedMultiply(first,second,multiply);
  39:simpleMMM.c   **** }
  40:simpleMMM.c   **** 
  41:simpleMMM.c   **** 
GAS LISTING /tmp/ccxAlghv.s 			page 2


  42:simpleMMM.c   **** void inline M4x4_SSE(float *A, float *B, float *C) {
  43:simpleMMM.c   ****     __m128 row1 = _mm_load_ps(&B[0]);
  44:simpleMMM.c   ****     __m128 row2 = _mm_load_ps(&B[4]);
  45:simpleMMM.c   ****     __m128 row3 = _mm_load_ps(&B[8]);
  46:simpleMMM.c   ****     __m128 row4 = _mm_load_ps(&B[12]);
  47:simpleMMM.c   ****     for(int i=0; i<4; i++) {
  48:simpleMMM.c   ****         __m128 brod1 = _mm_set1_ps(A[4*i + 0]);
  49:simpleMMM.c   ****         __m128 brod2 = _mm_set1_ps(A[4*i + 1]);
  50:simpleMMM.c   ****         __m128 brod3 = _mm_set1_ps(A[4*i + 2]);
  51:simpleMMM.c   ****         __m128 brod4 = _mm_set1_ps(A[4*i + 3]);
  52:simpleMMM.c   ****         __m128 row = _mm_add_ps(
  53:simpleMMM.c   ****                     _mm_add_ps(
  54:simpleMMM.c   ****                         _mm_mul_ps(brod1, row1),
  55:simpleMMM.c   ****                         _mm_mul_ps(brod2, row2)),
  56:simpleMMM.c   ****                     _mm_add_ps(
  57:simpleMMM.c   ****                         _mm_mul_ps(brod3, row3),
  58:simpleMMM.c   ****                         _mm_mul_ps(brod4, row4)));
  59:simpleMMM.c   ****         _mm_store_ps(&C[4*i], row);
  60:simpleMMM.c   ****     }
  61:simpleMMM.c   **** }
  62:simpleMMM.c   **** 
  63:simpleMMM.c   **** 
  64:simpleMMM.c   **** 
  65:simpleMMM.c   **** void blockedMultiply(double * restrict A, double * restrict B, double * restrict C)
  66:simpleMMM.c   **** {
  14              		.loc 1 66 0
  15              		.cfi_startproc
  16              	.LVL0:
  17 0000 4157     		pushq	%r15
  18              	.LCFI0:
  19              		.cfi_def_cfa_offset 16
  20              		.cfi_offset 15, -16
  21 0002 4156     		pushq	%r14
  22              	.LCFI1:
  23              		.cfi_def_cfa_offset 24
  24              		.cfi_offset 14, -24
  25 0004 4155     		pushq	%r13
  26              	.LCFI2:
  27              		.cfi_def_cfa_offset 32
  28              		.cfi_offset 13, -32
  29 0006 4154     		pushq	%r12
  30              	.LCFI3:
  31              		.cfi_def_cfa_offset 40
  32              		.cfi_offset 12, -40
  33 0008 55       		pushq	%rbp
  34              	.LCFI4:
  35              		.cfi_def_cfa_offset 48
  36              		.cfi_offset 6, -48
  37 0009 4889D5   		movq	%rdx, %rbp
  38 000c 53       		pushq	%rbx
  39              	.LCFI5:
  40              		.cfi_def_cfa_offset 56
  41              		.cfi_offset 3, -56
  42 000d 4889FB   		movq	%rdi, %rbx
  67:simpleMMM.c   ****   struct timespec start, end;
  68:simpleMMM.c   ****   clock_gettime(CLOCK_MONOTONIC, &start);
  43              		.loc 1 68 0
GAS LISTING /tmp/ccxAlghv.s 			page 3


  44 0010 BF010000 		movl	$1, %edi
  44      00
  45              	.LVL1:
  66:simpleMMM.c   **** {
  46              		.loc 1 66 0
  47 0015 4883EC78 		subq	$120, %rsp
  48              	.LCFI6:
  49              		.cfi_def_cfa_offset 176
  66:simpleMMM.c   **** {
  50              		.loc 1 66 0
  51 0019 48897424 		movq	%rsi, 56(%rsp)
  51      38
  52              		.loc 1 68 0
  53 001e 488D7424 		leaq	80(%rsp), %rsi
  53      50
  54              	.LVL2:
  55 0023 E8000000 		call	clock_gettime
  55      00
  56              	.LVL3:
  69:simpleMMM.c   **** 
  70:simpleMMM.c   ****   const int NB =  40;
  71:simpleMMM.c   ****   double sum;
  72:simpleMMM.c   **** 
  73:simpleMMM.c   ****   double *X = __builtin_assume_aligned(A, 16);
  57              		.loc 1 73 0
  58 0028 4889DF   		movq	%rbx, %rdi
  59 002b BE100000 		movl	$16, %esi
  59      00
  60 0030 31C0     		xorl	%eax, %eax
  61 0032 E8000000 		call	__builtin_assume_aligned
  61      00
  74:simpleMMM.c   ****   double *Y = __builtin_assume_aligned(B, 16);
  62              		.loc 1 74 0
  63 0037 488B7C24 		movq	56(%rsp), %rdi
  63      38
  64 003c BE100000 		movl	$16, %esi
  64      00
  65 0041 31C0     		xorl	%eax, %eax
  66              	.LBB13:
  75:simpleMMM.c   ****   double *Z = __builtin_assume_aligned(C, 16);
  76:simpleMMM.c   **** 
  77:simpleMMM.c   ****   for(int i=0; i<ndim; i+=NB)
  67              		.loc 1 77 0
  68 0043 4829EB   		subq	%rbp, %rbx
  69              	.LVL4:
  70              	.LBE13:
  74:simpleMMM.c   ****   double *Y = __builtin_assume_aligned(B, 16);
  71              		.loc 1 74 0
  72 0046 E8000000 		call	__builtin_assume_aligned
  72      00
  75:simpleMMM.c   ****   double *Z = __builtin_assume_aligned(C, 16);
  73              		.loc 1 75 0
  74 004b BE100000 		movl	$16, %esi
  74      00
  75 0050 4889EF   		movq	%rbp, %rdi
  76 0053 31C0     		xorl	%eax, %eax
  77 0055 E8000000 		call	__builtin_assume_aligned
GAS LISTING /tmp/ccxAlghv.s 			page 4


  77      00
  78              	.LVL5:
  65:simpleMMM.c   **** void blockedMultiply(double * restrict A, double * restrict B, double * restrict C)
  79              		.loc 1 65 0
  80 005a 4889E8   		movq	%rbp, %rax
  81 005d 482B4424 		subq	56(%rsp), %rax
  81      38
  82              	.LBB44:
  83              	.LBB14:
  84              		.loc 1 77 0
  85 0062 C7442418 		movl	$0, 24(%rsp)
  85      00000000 
  86              	.LBE14:
  87 006a 48895C24 		movq	%rbx, 72(%rsp)
  87      48
  88 006f 4531C9   		xorl	%r9d, %r9d
  89              	.LBE44:
  65:simpleMMM.c   **** void blockedMultiply(double * restrict A, double * restrict B, double * restrict C)
  90              		.loc 1 65 0
  91 0072 48894424 		movq	%rax, 48(%rsp)
  91      30
  92 0077 8B442418 		movl	24(%rsp), %eax
  93              	.LVL6:
  94              	.L2:
  95              	.LBB45:
  96              		.loc 1 77 0 discriminator 1
  97 007b 83C028   		addl	$40, %eax
  98 007e 817C2418 		cmpl	$1959, 24(%rsp)
  98      A7070000 
  99 0086 89442420 		movl	%eax, 32(%rsp)
 100 008a B8D00700 		movl	$2000, %eax
 100      00
 101 008f 0F4E4424 		cmovle	32(%rsp), %eax
 101      20
 102              	.LBB40:
 103              	.LBB15:
 104              	.LBB16:
 105              	.LBB17:
 106              	.LBB18:
 107              	.LBB19:
  65:simpleMMM.c   **** void blockedMultiply(double * restrict A, double * restrict B, double * restrict C)
 108              		.loc 1 65 0 discriminator 1
 109 0094 4531ED   		xorl	%r13d, %r13d
 110              	.LBE19:
 111              	.LBE18:
 112              	.LBE17:
 113              	.LBE16:
 114              	.LBE15:
 115              	.LBE40:
 116              		.loc 1 77 0 discriminator 1
 117 0097 8944240C 		movl	%eax, 12(%rsp)
 118 009b 488B4424 		movq	72(%rsp), %rax
 118      48
 119 00a0 48894424 		movq	%rax, 40(%rsp)
 119      28
 120              	.LBB41:
 121              	.LBB38:
GAS LISTING /tmp/ccxAlghv.s 			page 5


 122              	.LBB36:
 123              	.LBB31:
 124              	.LBB25:
 125              	.LBB20:
  65:simpleMMM.c   **** void blockedMultiply(double * restrict A, double * restrict B, double * restrict C)
 126              		.loc 1 65 0 discriminator 1
 127 00a5 488B4424 		movq	56(%rsp), %rax
 127      38
 128 00aa 48894424 		movq	%rax, 16(%rsp)
 128      10
 129 00af 4889C5   		movq	%rax, %rbp
 130              	.LVL7:
 131              	.L17:
 132              	.LBE20:
 133              	.LBE25:
 134              	.LBE31:
 135              	.LBE36:
 136              	.LBE38:
  78:simpleMMM.c   ****     for(int j=0; j<ndim; j+=NB)
 137              		.loc 1 78 0 discriminator 1
 138 00b2 418D4528 		leal	40(%r13), %eax
 139 00b6 4181FDA7 		cmpl	$1959, %r13d
 139      070000
 140              	.LBE41:
 141              	.LBE45:
  66:simpleMMM.c   **** {
 142              		.loc 1 66 0 discriminator 1
 143 00bd 4C8B7424 		movq	40(%rsp), %r14
 143      28
 144 00c2 C744241C 		movl	$40, 28(%rsp)
 144      28000000 
 145              	.LBB46:
 146              	.LBB42:
 147              		.loc 1 78 0 discriminator 1
 148 00ca 89442424 		movl	%eax, 36(%rsp)
 149 00ce B8D00700 		movl	$2000, %eax
 149      00
 150 00d3 0F4E4424 		cmovle	36(%rsp), %eax
 150      24
 151              	.LBE42:
 152              	.LBE46:
  66:simpleMMM.c   **** {
 153              		.loc 1 66 0 discriminator 1
 154 00d8 4531D2   		xorl	%r10d, %r10d
 155              	.LBB47:
 156              	.LBB43:
 157              		.loc 1 78 0 discriminator 1
 158 00db 89442408 		movl	%eax, 8(%rsp)
 159              	.LBB39:
 160              	.LBB37:
 161              	.LBB32:
  65:simpleMMM.c   **** void blockedMultiply(double * restrict A, double * restrict B, double * restrict C)
 162              		.loc 1 65 0 discriminator 1
 163 00df 4889E8   		movq	%rbp, %rax
 164 00e2 48034424 		addq	48(%rsp), %rax
 164      30
 165 00e7 48894424 		movq	%rax, 64(%rsp)
GAS LISTING /tmp/ccxAlghv.s 			page 6


 165      40
 166              	.LBB26:
 167 00ec 8B442408 		movl	8(%rsp), %eax
 168 00f0 83E801   		subl	$1, %eax
 169 00f3 4429E8   		subl	%r13d, %eax
 170 00f6 488D1CC5 		leaq	8(,%rax,8), %rbx
 170      08000000 
 171              	.LVL8:
 172              	.L10:
 173              	.LBE26:
 174              	.LBE32:
  79:simpleMMM.c   ****       for(int k=0; k<ndim; k+=NB)
  80:simpleMMM.c   ****       {
  81:simpleMMM.c   ****         int i_max = (i+NB < ndim)? i+NB : ndim;
  82:simpleMMM.c   ****         int j_max = (j+NB < ndim)? j+NB : ndim;
  83:simpleMMM.c   ****         int k_max = (k+NB < ndim)? k+NB : ndim;
 175              		.loc 1 83 0
 176 00fe 4181FAA8 		cmpl	$1960, %r10d
 176      070000
 177 0105 41BBD007 		movl	$2000, %r11d
 177      0000
 178              	.LBB33:
  84:simpleMMM.c   ****         for(int i0=i; i0<i_max; i0++)
 179              		.loc 1 84 0
 180 010b 8B442418 		movl	24(%rsp), %eax
 181              	.LBE33:
  83:simpleMMM.c   ****         int k_max = (k+NB < ndim)? k+NB : ndim;
 182              		.loc 1 83 0
 183 010f 440F455C 		cmovne	28(%rsp), %r11d
 183      241C
 184              	.LVL9:
 185              	.LBB34:
 186              		.loc 1 84 0
 187 0115 3944240C 		cmpl	%eax, 12(%rsp)
 188 0119 0F8E8D00 		jle	.L4
 188      0000
 189              	.LBB27:
 190              	.LBB21:
  65:simpleMMM.c   **** void blockedMultiply(double * restrict A, double * restrict B, double * restrict C)
 191              		.loc 1 65 0
 192 011f 4589D7   		movl	%r10d, %r15d
 193              	.LBE21:
 194              	.LBE27:
 195 0122 4C8B4424 		movq	64(%rsp), %r8
 195      40
 196              	.LBB28:
 197              	.LBB22:
 198 0127 4189C4   		movl	%eax, %r12d
 199 012a 41F7D7   		notl	%r15d
 200 012d 4501DF   		addl	%r11d, %r15d
 201 0130 4983C701 		addq	$1, %r15
 202              	.LVL10:
 203              	.L5:
 204              	.LBE22:
  85:simpleMMM.c   ****           for(int j0=j; j0<j_max; j0++)
 205              		.loc 1 85 0 discriminator 1
 206 0134 44396C24 		cmpl	%r13d, 8(%rsp)
GAS LISTING /tmp/ccxAlghv.s 			page 7


 206      08
 207 0139 7E5F     		jle	.L7
 208              	.LBB23:
  65:simpleMMM.c   **** void blockedMultiply(double * restrict A, double * restrict B, double * restrict C)
 209              		.loc 1 65 0
 210 013b 4A8D34FD 		leaq	0(,%r15,8), %rsi
 210      00000000 
 211 0143 4B8D0C06 		leaq	(%r14,%r8), %rcx
 212 0147 31FF     		xorl	%edi, %edi
 213              	.LVL11:
 214 0149 0F1F8000 		.p2align 4,,10
 214      000000
 215              		.p2align 3
 216              	.L9:
  86:simpleMMM.c   ****             {
  87:simpleMMM.c   **** 
  88:simpleMMM.c   ****               sum = 0;
  89:simpleMMM.c   ****               for(int k0=k; k0<k_max; k0++)
 217              		.loc 1 89 0 discriminator 1
 218 0150 4C890C24 		movq	%r9, (%rsp)
 219              	.LBE23:
 220              	.LBE28:
 221              	.LBE34:
  65:simpleMMM.c   **** void blockedMultiply(double * restrict A, double * restrict B, double * restrict C)
 222              		.loc 1 65 0 discriminator 1
 223 0154 31C0     		xorl	%eax, %eax
 224              	.LBB35:
 225              	.LBB29:
 226              	.LBB24:
 227              		.loc 1 89 0 discriminator 1
 228 0156 4539D3   		cmpl	%r10d, %r11d
 229 0159 F20F100C 		movsd	(%rsp), %xmm1
 229      24
  65:simpleMMM.c   **** void blockedMultiply(double * restrict A, double * restrict B, double * restrict C)
 230              		.loc 1 65 0 discriminator 1
 231 015e 488D142F 		leaq	(%rdi,%rbp), %rdx
 232              		.loc 1 89 0 discriminator 1
 233 0162 7E21     		jle	.L8
 234              	.LVL12:
 235              		.p2align 4,,10
 236 0164 0F1F4000 		.p2align 3
 237              	.L6:
  90:simpleMMM.c   ****                 sum += A[IDX(i0,k0)] * B[IDX(k0,j0)];
 238              		.loc 1 90 0 discriminator 2
 239 0168 F20F1004 		movsd	(%rcx,%rax), %xmm0
 239      01
 240 016d 4883C008 		addq	$8, %rax
 241 0171 F20F5902 		mulsd	(%rdx), %xmm0
 242 0175 4881C280 		addq	$16000, %rdx
 242      3E0000
  89:simpleMMM.c   ****               for(int k0=k; k0<k_max; k0++)
 243              		.loc 1 89 0 discriminator 2
 244 017c 4839F0   		cmpq	%rsi, %rax
 245              		.loc 1 90 0 discriminator 2
 246 017f F20F58C8 		addsd	%xmm0, %xmm1
 247              	.LVL13:
  89:simpleMMM.c   ****               for(int k0=k; k0<k_max; k0++)
GAS LISTING /tmp/ccxAlghv.s 			page 8


 248              		.loc 1 89 0 discriminator 2
 249 0183 75E3     		jne	.L6
 250              	.LVL14:
 251              	.L8:
 252              	.LBE24:
  91:simpleMMM.c   ****               C[IDX(i0,j0)] += sum;
 253              		.loc 1 91 0
 254 0185 F2410F58 		addsd	(%r8,%rdi), %xmm1
 254      0C38
 255 018b F2410F11 		movsd	%xmm1, (%r8,%rdi)
 255      0C38
 256 0191 4883C708 		addq	$8, %rdi
  85:simpleMMM.c   ****           for(int j0=j; j0<j_max; j0++)
 257              		.loc 1 85 0
 258 0195 4839DF   		cmpq	%rbx, %rdi
 259 0198 75B6     		jne	.L9
 260              	.L7:
 261              	.LBE29:
  84:simpleMMM.c   ****         for(int i0=i; i0<i_max; i0++)
 262              		.loc 1 84 0
 263 019a 4183C401 		addl	$1, %r12d
 264              	.LVL15:
 265 019e 4981C080 		addq	$16000, %r8
 265      3E0000
 266 01a5 443B6424 		cmpl	12(%rsp), %r12d
 266      0C
 267 01aa 7588     		jne	.L5
 268              	.LVL16:
 269              	.L4:
 270              	.LBB30:
  65:simpleMMM.c   **** void blockedMultiply(double * restrict A, double * restrict B, double * restrict C)
 271              		.loc 1 65 0
 272 01ac 4183C228 		addl	$40, %r10d
 273 01b0 8344241C 		addl	$40, 28(%rsp)
 273      28
 274 01b5 4981C640 		addq	$320, %r14
 274      010000
 275 01bc 4881C500 		addq	$640000, %rbp
 275      C40900
 276              	.LBE30:
 277              	.LBE35:
 278              	.LBE37:
  79:simpleMMM.c   ****       for(int k=0; k<ndim; k+=NB)
 279              		.loc 1 79 0
 280 01c3 4181FAD0 		cmpl	$2000, %r10d
 280      070000
 281 01ca 0F852EFF 		jne	.L10
 281      FFFF
 282              	.LVL17:
 283 01d0 48816C24 		subq	$320, 40(%rsp)
 283      28400100 
 283      00
 284 01d9 48814424 		addq	$320, 16(%rsp)
 284      10400100 
 284      00
 285              	.LBE39:
  78:simpleMMM.c   ****     for(int j=0; j<ndim; j+=NB)
GAS LISTING /tmp/ccxAlghv.s 			page 9


 286              		.loc 1 78 0
 287 01e2 817C2424 		cmpl	$2000, 36(%rsp)
 287      D0070000 
 288 01ea 740F     		je	.L11
 289 01ec 448B6C24 		movl	36(%rsp), %r13d
 289      24
 290 01f1 488B6C24 		movq	16(%rsp), %rbp
 290      10
 291 01f6 E9B7FEFF 		jmp	.L17
 291      FF
 292              	.L11:
 293              	.LVL18:
 294 01fb 48814424 		addq	$640000, 48(%rsp)
 294      3000C409 
 294      00
 295              	.LBE43:
  77:simpleMMM.c   ****   for(int i=0; i<ndim; i+=NB)
 296              		.loc 1 77 0
 297 0204 817C2420 		cmpl	$2000, 32(%rsp)
 297      D0070000 
 298 020c 740D     		je	.L14
 299 020e 8B442420 		movl	32(%rsp), %eax
 300 0212 89442418 		movl	%eax, 24(%rsp)
 301 0216 E960FEFF 		jmp	.L2
 301      FF
 302              	.L14:
 303              	.LBE47:
  92:simpleMMM.c   ****             }
  93:simpleMMM.c   ****         }
  94:simpleMMM.c   **** 
  95:simpleMMM.c   **** 
  96:simpleMMM.c   ****   clock_gettime(CLOCK_MONOTONIC, &end);
 304              		.loc 1 96 0
 305 021b 488D7424 		leaq	96(%rsp), %rsi
 305      60
 306 0220 BF010000 		movl	$1, %edi
 306      00
 307 0225 E8000000 		call	clock_gettime
 307      00
 308              	.LVL19:
  97:simpleMMM.c   ****   float seconds = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / BILLION;
 309              		.loc 1 97 0
 310 022a 488B4424 		movq	96(%rsp), %rax
 310      60
 311 022f 482B4424 		subq	80(%rsp), %rax
 311      50
 312              	.LBB48:
 313              	.LBB49:
 314              		.file 2 "/usr/include/x86_64-linux-gnu/bits/stdio2.h"
   1:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** /* Checking macros for stdio functions.
   2:/usr/include/x86_64-linux-gnu/bits/stdio2.h ****    Copyright (C) 2004, 2005, 2007, 2008 Free Software Foundation, Inc.
   3:/usr/include/x86_64-linux-gnu/bits/stdio2.h ****    This file is part of the GNU C Library.
   4:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** 
   5:/usr/include/x86_64-linux-gnu/bits/stdio2.h ****    The GNU C Library is free software; you can redistribute it and/or
   6:/usr/include/x86_64-linux-gnu/bits/stdio2.h ****    modify it under the terms of the GNU Lesser General Public
   7:/usr/include/x86_64-linux-gnu/bits/stdio2.h ****    License as published by the Free Software Foundation; either
   8:/usr/include/x86_64-linux-gnu/bits/stdio2.h ****    version 2.1 of the License, or (at your option) any later version.
GAS LISTING /tmp/ccxAlghv.s 			page 10


   9:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** 
  10:/usr/include/x86_64-linux-gnu/bits/stdio2.h ****    The GNU C Library is distributed in the hope that it will be useful,
  11:/usr/include/x86_64-linux-gnu/bits/stdio2.h ****    but WITHOUT ANY WARRANTY; without even the implied warranty of
  12:/usr/include/x86_64-linux-gnu/bits/stdio2.h ****    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  13:/usr/include/x86_64-linux-gnu/bits/stdio2.h ****    Lesser General Public License for more details.
  14:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** 
  15:/usr/include/x86_64-linux-gnu/bits/stdio2.h ****    You should have received a copy of the GNU Lesser General Public
  16:/usr/include/x86_64-linux-gnu/bits/stdio2.h ****    License along with the GNU C Library; if not, write to the Free
  17:/usr/include/x86_64-linux-gnu/bits/stdio2.h ****    Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
  18:/usr/include/x86_64-linux-gnu/bits/stdio2.h ****    02111-1307 USA.  */
  19:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** 
  20:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** #ifndef _STDIO_H
  21:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** # error "Never include <bits/stdio2.h> directly; use <stdio.h> instead."
  22:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** #endif
  23:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** 
  24:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** extern int __sprintf_chk (char *__restrict __s, int __flag, size_t __slen,
  25:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** 			  __const char *__restrict __format, ...) __THROW;
  26:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** extern int __vsprintf_chk (char *__restrict __s, int __flag, size_t __slen,
  27:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** 			   __const char *__restrict __format,
  28:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** 			   _G_va_list __ap) __THROW;
  29:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** 
  30:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** #ifdef __va_arg_pack
  31:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** __extern_always_inline int
  32:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** __NTH (sprintf (char *__restrict __s, __const char *__restrict __fmt, ...))
  33:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** {
  34:/usr/include/x86_64-linux-gnu/bits/stdio2.h ****   return __builtin___sprintf_chk (__s, __USE_FORTIFY_LEVEL - 1,
  35:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** 				  __bos (__s), __fmt, __va_arg_pack ());
  36:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** }
  37:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** #elif !defined __cplusplus
  38:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** # define sprintf(str, ...) \
  39:/usr/include/x86_64-linux-gnu/bits/stdio2.h ****   __builtin___sprintf_chk (str, __USE_FORTIFY_LEVEL - 1, __bos (str), \
  40:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** 			   __VA_ARGS__)
  41:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** #endif
  42:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** 
  43:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** __extern_always_inline int
  44:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** __NTH (vsprintf (char *__restrict __s, __const char *__restrict __fmt,
  45:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** 		 _G_va_list __ap))
  46:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** {
  47:/usr/include/x86_64-linux-gnu/bits/stdio2.h ****   return __builtin___vsprintf_chk (__s, __USE_FORTIFY_LEVEL - 1,
  48:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** 				   __bos (__s), __fmt, __ap);
  49:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** }
  50:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** 
  51:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** #if defined __USE_BSD || defined __USE_ISOC99 || defined __USE_UNIX98
  52:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** 
  53:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** extern int __snprintf_chk (char *__restrict __s, size_t __n, int __flag,
  54:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** 			   size_t __slen, __const char *__restrict __format,
  55:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** 			   ...) __THROW;
  56:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** extern int __vsnprintf_chk (char *__restrict __s, size_t __n, int __flag,
  57:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** 			    size_t __slen, __const char *__restrict __format,
  58:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** 			    _G_va_list __ap) __THROW;
  59:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** 
  60:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** # ifdef __va_arg_pack
  61:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** __extern_always_inline int
  62:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** __NTH (snprintf (char *__restrict __s, size_t __n,
  63:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** 		 __const char *__restrict __fmt, ...))
  64:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** {
  65:/usr/include/x86_64-linux-gnu/bits/stdio2.h ****   return __builtin___snprintf_chk (__s, __n, __USE_FORTIFY_LEVEL - 1,
GAS LISTING /tmp/ccxAlghv.s 			page 11


  66:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** 				   __bos (__s), __fmt, __va_arg_pack ());
  67:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** }
  68:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** # elif !defined __cplusplus
  69:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** #  define snprintf(str, len, ...) \
  70:/usr/include/x86_64-linux-gnu/bits/stdio2.h ****   __builtin___snprintf_chk (str, len, __USE_FORTIFY_LEVEL - 1, __bos (str), \
  71:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** 			    __VA_ARGS__)
  72:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** # endif
  73:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** 
  74:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** __extern_always_inline int
  75:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** __NTH (vsnprintf (char *__restrict __s, size_t __n,
  76:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** 		  __const char *__restrict __fmt, _G_va_list __ap))
  77:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** {
  78:/usr/include/x86_64-linux-gnu/bits/stdio2.h ****   return __builtin___vsnprintf_chk (__s, __n, __USE_FORTIFY_LEVEL - 1,
  79:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** 				    __bos (__s), __fmt, __ap);
  80:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** }
  81:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** 
  82:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** #endif
  83:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** 
  84:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** #if __USE_FORTIFY_LEVEL > 1
  85:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** 
  86:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** extern int __fprintf_chk (FILE *__restrict __stream, int __flag,
  87:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** 			  __const char *__restrict __format, ...);
  88:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** extern int __printf_chk (int __flag, __const char *__restrict __format, ...);
  89:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** extern int __vfprintf_chk (FILE *__restrict __stream, int __flag,
  90:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** 			   __const char *__restrict __format, _G_va_list __ap);
  91:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** extern int __vprintf_chk (int __flag, __const char *__restrict __format,
  92:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** 			  _G_va_list __ap);
  93:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** 
  94:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** # ifdef __va_arg_pack
  95:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** __extern_always_inline int
  96:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** fprintf (FILE *__restrict __stream, __const char *__restrict __fmt, ...)
  97:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** {
  98:/usr/include/x86_64-linux-gnu/bits/stdio2.h ****   return __fprintf_chk (__stream, __USE_FORTIFY_LEVEL - 1, __fmt,
  99:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** 			__va_arg_pack ());
 100:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** }
 101:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** 
 102:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** __extern_always_inline int
 103:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** printf (__const char *__restrict __fmt, ...)
 104:/usr/include/x86_64-linux-gnu/bits/stdio2.h **** {
 105:/usr/include/x86_64-linux-gnu/bits/stdio2.h ****   return __printf_chk (__USE_FORTIFY_LEVEL - 1, __fmt, __va_arg_pack ());
 315              		.loc 2 105 0
 316 0234 BE000000 		movl	$.LC2, %esi
 316      00
 317 0239 BF010000 		movl	$1, %edi
 317      00
 318              	.LBE49:
 319              	.LBE48:
 320              		.loc 1 97 0
 321 023e F2480F2A 		cvtsi2sdq	%rax, %xmm0
 321      C0
 322 0243 488B4424 		movq	104(%rsp), %rax
 322      68
 323 0248 482B4424 		subq	88(%rsp), %rax
 323      58
 324 024d F2480F2A 		cvtsi2sdq	%rax, %xmm1
 324      C8
 325              	.LBB52:
GAS LISTING /tmp/ccxAlghv.s 			page 12


 326              	.LBB50:
 327              		.loc 2 105 0
 328 0252 B8010000 		movl	$1, %eax
 328      00
 329              	.LBE50:
 330              	.LBE52:
 331              		.loc 1 97 0
 332 0257 F20F5E0D 		divsd	.LC1(%rip), %xmm1
 332      00000000 
 333 025f F20F58C1 		addsd	%xmm1, %xmm0
 334 0263 660F14C0 		unpcklpd	%xmm0, %xmm0
 335 0267 660F5AC0 		cvtpd2ps	%xmm0, %xmm0
  98:simpleMMM.c   **** 
  99:simpleMMM.c   ****   printf("blocked took: %f\n\n", seconds);
 336              		.loc 1 99 0
 337 026b 0F14C0   		unpcklps	%xmm0, %xmm0
 338 026e 0F5AC0   		cvtps2pd	%xmm0, %xmm0
 339              	.LBB53:
 340              	.LBB51:
 341              		.loc 2 105 0
 342 0271 E8000000 		call	__printf_chk
 342      00
 343              	.LBE51:
 344              	.LBE53:
 100:simpleMMM.c   **** }...
 345              		.loc 1 100 0
 346 0276 4883C478 		addq	$120, %rsp
 347              	.LCFI7:
 348              		.cfi_def_cfa_offset 56
 349 027a 5B       		popq	%rbx
 350              	.LCFI8:
 351              		.cfi_def_cfa_offset 48
 352 027b 5D       		popq	%rbp
 353              	.LCFI9:
 354              		.cfi_def_cfa_offset 40
 355 027c 415C     		popq	%r12
 356              	.LCFI10:
 357              		.cfi_def_cfa_offset 32
 358 027e 415D     		popq	%r13
 359              	.LCFI11:
 360              		.cfi_def_cfa_offset 24
 361 0280 415E     		popq	%r14
 362              	.LCFI12:
 363              		.cfi_def_cfa_offset 16
 364 0282 415F     		popq	%r15
 365              	.LCFI13:
 366              		.cfi_def_cfa_offset 8
 367 0284 C3       		ret
 368              		.cfi_endproc
 369              	.LFE545:
 371              		.section	.rodata.str1.1
 372              	.LC3:
 373 0013 6E64696D 		.string	"ndim: %d\n"
 373      3A202564 
 373      0A00
 374              		.section	.text.startup,"ax",@progbits
 375              		.p2align 4,,15
GAS LISTING /tmp/ccxAlghv.s 			page 13


 376              		.globl	main
 378              	main:
 379              	.LFB543:
  15:simpleMMM.c   **** {
 380              		.loc 1 15 0
 381              		.cfi_startproc
 382              	.LVL20:
 383 0000 4157     		pushq	%r15
 384              	.LCFI14:
 385              		.cfi_def_cfa_offset 16
 386              		.cfi_offset 15, -16
  18:simpleMMM.c   ****     srand(1);
 387              		.loc 1 18 0
 388 0002 BF010000 		movl	$1, %edi
 388      00
 389              	.LVL21:
  15:simpleMMM.c   **** {
 390              		.loc 1 15 0
 391 0007 4156     		pushq	%r14
 392              	.LCFI15:
 393              		.cfi_def_cfa_offset 24
 394              		.cfi_offset 14, -24
 395 0009 4155     		pushq	%r13
 396              	.LCFI16:
 397              		.cfi_def_cfa_offset 32
 398              		.cfi_offset 13, -32
  31:simpleMMM.c   ****             first[IDX(c, d)] = rand() % 10; //int between 0 - 1000
 399              		.loc 1 31 0
 400 000b 41BD6766 		movl	$1717986919, %r13d
 400      6666
  15:simpleMMM.c   **** {
 401              		.loc 1 15 0
 402 0011 4154     		pushq	%r12
 403              	.LCFI17:
 404              		.cfi_def_cfa_offset 40
 405              		.cfi_offset 12, -40
 406 0013 55       		pushq	%rbp
 407              	.LCFI18:
 408              		.cfi_def_cfa_offset 48
 409              		.cfi_offset 6, -48
 410 0014 53       		pushq	%rbx
 411              	.LCFI19:
 412              		.cfi_def_cfa_offset 56
 413              		.cfi_offset 3, -56
 414 0015 4883EC38 		subq	$56, %rsp
 415              	.LCFI20:
 416              		.cfi_def_cfa_offset 112
  18:simpleMMM.c   ****     srand(1);
 417              		.loc 1 18 0
 418 0019 E8000000 		call	srand
 418      00
 419              	.LVL22:
  19:simpleMMM.c   ****     double *first = malloc(ndim * ndim * sizeof(double));
 420              		.loc 1 19 0
 421 001e BF0048E8 		movl	$32000000, %edi
 421      01
 422 0023 E8000000 		call	malloc
GAS LISTING /tmp/ccxAlghv.s 			page 14


 422      00
  20:simpleMMM.c   ****     double *second = malloc(ndim * ndim * sizeof(double));
 423              		.loc 1 20 0
 424 0028 BF0048E8 		movl	$32000000, %edi
 424      01
  19:simpleMMM.c   ****     double *first = malloc(ndim * ndim * sizeof(double));
 425              		.loc 1 19 0
 426 002d 4889C3   		movq	%rax, %rbx
 427              	.LVL23:
  20:simpleMMM.c   ****     double *second = malloc(ndim * ndim * sizeof(double));
 428              		.loc 1 20 0
 429 0030 E8000000 		call	malloc
 429      00
 430              	.LVL24:
  21:simpleMMM.c   ****     double *multiply = malloc(ndim * ndim * sizeof(double));
 431              		.loc 1 21 0
 432 0035 BF0048E8 		movl	$32000000, %edi
 432      01
  20:simpleMMM.c   ****     double *second = malloc(ndim * ndim * sizeof(double));
 433              		.loc 1 20 0
 434 003a 48894424 		movq	%rax, 40(%rsp)
 434      28
 435              	.LVL25:
  24:simpleMMM.c   ****     setbuf(stdout, NULL);
 436              		.loc 1 24 0
 437 003f 4989DE   		movq	%rbx, %r14
  21:simpleMMM.c   ****     double *multiply = malloc(ndim * ndim * sizeof(double));
 438              		.loc 1 21 0
 439 0042 E8000000 		call	malloc
 439      00
 440              	.LVL26:
 441              	.LBB54:
 442              	.LBB55:
 443              		.loc 2 105 0
 444 0047 BAD00700 		movl	$2000, %edx
 444      00
 445              	.LBE55:
 446              	.LBE54:
  21:simpleMMM.c   ****     double *multiply = malloc(ndim * ndim * sizeof(double));
 447              		.loc 1 21 0
 448 004c 4889C5   		movq	%rax, %rbp
 449              	.LVL27:
 450              	.LBB57:
 451              	.LBB56:
 452              		.loc 2 105 0
 453 004f BE000000 		movl	$.LC3, %esi
 453      00
 454 0054 BF010000 		movl	$1, %edi
 454      00
 455 0059 31C0     		xorl	%eax, %eax
 456              	.LVL28:
 457 005b E8000000 		call	__printf_chk
 457      00
 458              	.LBE56:
 459              	.LBE57:
  24:simpleMMM.c   ****     setbuf(stdout, NULL);
 460              		.loc 1 24 0
GAS LISTING /tmp/ccxAlghv.s 			page 15


 461 0060 488B3D00 		movq	stdout(%rip), %rdi
 461      000000
 462 0067 31F6     		xorl	%esi, %esi
 463 0069 E8000000 		call	setbuf
 463      00
 464              	.LVL29:
  14:simpleMMM.c   **** int main(int argc, char **argv)
 465              		.loc 1 14 0
 466 006e 488B4424 		movq	40(%rsp), %rax
 466      28
  24:simpleMMM.c   ****     setbuf(stdout, NULL);
 467              		.loc 1 24 0
 468 0073 C7442414 		movl	$2000, 20(%rsp)
 468      D0070000 
  14:simpleMMM.c   **** int main(int argc, char **argv)
 469              		.loc 1 14 0
 470 007b 4829D8   		subq	%rbx, %rax
 471 007e 48894424 		movq	%rax, 24(%rsp)
 471      18
 472 0083 4889E8   		movq	%rbp, %rax
 473 0086 4829D8   		subq	%rbx, %rax
 474 0089 48894424 		movq	%rax, 32(%rsp)
 474      20
 475              	.LVL30:
 476 008e 6690     		.p2align 4,,10
 477              		.p2align 3
 478              	.L26:
  14:simpleMMM.c   **** int main(int argc, char **argv)
 479              		.loc 1 14 0 is_stmt 0 discriminator 1
 480 0090 488B4424 		movq	24(%rsp), %rax
 480      18
 481 0095 4C8B7C24 		movq	32(%rsp), %r15
 481      20
  15:simpleMMM.c   **** {
 482              		.loc 1 15 0 is_stmt 1 discriminator 1
 483 009a 4531E4   		xorl	%r12d, %r12d
  14:simpleMMM.c   **** int main(int argc, char **argv)
 484              		.loc 1 14 0 discriminator 1
 485 009d 4C01F0   		addq	%r14, %rax
 486 00a0 4D01F7   		addq	%r14, %r15
 487 00a3 48894424 		movq	%rax, 8(%rsp)
 487      08
 488              	.LVL31:
 489 00a8 0F1F8400 		.p2align 4,,10
 489      00000000 
 490              		.p2align 3
 491              	.L27:
  31:simpleMMM.c   ****             first[IDX(c, d)] = rand() % 10; //int between 0 - 1000
 492              		.loc 1 31 0 discriminator 2
 493 00b0 E8000000 		call	rand
 493      00
 494 00b5 89C1     		movl	%eax, %ecx
 495 00b7 41F7ED   		imull	%r13d
 496 00ba 89C8     		movl	%ecx, %eax
 497 00bc C1F81F   		sarl	$31, %eax
 498 00bf C1FA02   		sarl	$2, %edx
 499 00c2 29C2     		subl	%eax, %edx
GAS LISTING /tmp/ccxAlghv.s 			page 16


 500 00c4 8D1492   		leal	(%rdx,%rdx,4), %edx
 501 00c7 01D2     		addl	%edx, %edx
 502 00c9 29D1     		subl	%edx, %ecx
 503 00cb F20F2AC1 		cvtsi2sd	%ecx, %xmm0
 504 00cf F2430F11 		movsd	%xmm0, (%r14,%r12)
 504      0426
  32:simpleMMM.c   ****             second[IDX(c, d)] = rand() % 10;
 505              		.loc 1 32 0 discriminator 2
 506 00d5 E8000000 		call	rand
 506      00
 507 00da 89C1     		movl	%eax, %ecx
  33:simpleMMM.c   ****             multiply[IDX(c, d)] = 0;
 508              		.loc 1 33 0 discriminator 2
 509 00dc 4BC70427 		movq	$0, (%r15,%r12)
 509      00000000 
  32:simpleMMM.c   ****             second[IDX(c, d)] = rand() % 10;
 510              		.loc 1 32 0 discriminator 2
 511 00e4 41F7ED   		imull	%r13d
 512 00e7 89C8     		movl	%ecx, %eax
 513 00e9 C1F81F   		sarl	$31, %eax
 514 00ec C1FA02   		sarl	$2, %edx
 515 00ef 29C2     		subl	%eax, %edx
 516 00f1 488B4424 		movq	8(%rsp), %rax
 516      08
 517 00f6 8D1492   		leal	(%rdx,%rdx,4), %edx
 518 00f9 01D2     		addl	%edx, %edx
 519 00fb 29D1     		subl	%edx, %ecx
 520 00fd F20F2AC1 		cvtsi2sd	%ecx, %xmm0
 521 0101 F2420F11 		movsd	%xmm0, (%rax,%r12)
 521      0420
  33:simpleMMM.c   ****             multiply[IDX(c, d)] = 0;
 522              		.loc 1 33 0 discriminator 2
 523 0107 4983C408 		addq	$8, %r12
  29:simpleMMM.c   ****         for (d = 0 ; d < ndim; d++)
 524              		.loc 1 29 0 discriminator 2
 525 010b 4981FC80 		cmpq	$16000, %r12
 525      3E0000
 526 0112 759C     		jne	.L27
  29:simpleMMM.c   ****         for (d = 0 ; d < ndim; d++)
 527              		.loc 1 29 0 is_stmt 0
 528 0114 4981C680 		addq	$16000, %r14
 528      3E0000
  27:simpleMMM.c   ****     for (c = 0 ; c < ndim; c++)
 529              		.loc 1 27 0 is_stmt 1
 530 011b 836C2414 		subl	$1, 20(%rsp)
 530      01
 531 0120 0F856AFF 		jne	.L26
 531      FFFF
  38:simpleMMM.c   ****     blockedMultiply(first,second,multiply);
 532              		.loc 1 38 0
 533 0126 488B7424 		movq	40(%rsp), %rsi
 533      28
 534 012b 4889EA   		movq	%rbp, %rdx
 535 012e 4889DF   		movq	%rbx, %rdi
 536 0131 E8000000 		call	blockedMultiply
 536      00
  39:simpleMMM.c   **** }
GAS LISTING /tmp/ccxAlghv.s 			page 17


 537              		.loc 1 39 0
 538 0136 4883C438 		addq	$56, %rsp
 539              	.LCFI21:
 540              		.cfi_def_cfa_offset 56
 541 013a 31C0     		xorl	%eax, %eax
 542 013c 5B       		popq	%rbx
 543              	.LCFI22:
 544              		.cfi_def_cfa_offset 48
 545              	.LVL32:
 546 013d 5D       		popq	%rbp
 547              	.LCFI23:
 548              		.cfi_def_cfa_offset 40
 549              	.LVL33:
 550 013e 415C     		popq	%r12
 551              	.LCFI24:
 552              		.cfi_def_cfa_offset 32
 553 0140 415D     		popq	%r13
 554              	.LCFI25:
 555              		.cfi_def_cfa_offset 24
 556 0142 415E     		popq	%r14
 557              	.LCFI26:
 558              		.cfi_def_cfa_offset 16
 559 0144 415F     		popq	%r15
 560              	.LCFI27:
 561              		.cfi_def_cfa_offset 8
 562 0146 C3       		ret
 563              		.cfi_endproc
 564              	.LFE543:
 566              		.globl	ndim
 567              		.section	.rodata
 568              		.align 4
 571              	ndim:
 572 0000 D0070000 		.long	2000
 573              		.section	.rodata.cst8,"aM",@progbits,8
 574              		.align 8
 575              	.LC1:
 576 0000 00000000 		.long	0
 577 0004 65CDCD41 		.long	1104006501
 578              		.text
 579              	.Letext0:
 580              		.file 3 "/usr/lib/gcc/x86_64-linux-gnu/4.6/include/stddef.h"
 581              		.file 4 "/usr/include/x86_64-linux-gnu/bits/types.h"
 582              		.file 5 "/usr/include/time.h"
 583              		.file 6 "/usr/include/libio.h"
 584              		.file 7 "/usr/include/stdio.h"
GAS LISTING /tmp/ccxAlghv.s 			page 18


DEFINED SYMBOLS
                            *ABS*:0000000000000000 simpleMMM.c
     /tmp/ccxAlghv.s:11     .text:0000000000000000 blockedMultiply
     /tmp/ccxAlghv.s:378    .text.startup:0000000000000000 main
     /tmp/ccxAlghv.s:571    .rodata:0000000000000000 ndim
                     .rodata.cst8:0000000000000000 .LC1

UNDEFINED SYMBOLS
clock_gettime
__builtin_assume_aligned
__printf_chk
srand
malloc
stdout
setbuf
rand
