GAS LISTING /tmp/cc1fPPOx.s 			page 1


   1              		.file	"assembler-experiment.c"
   2              		.text
   3              	.Ltext0:
   4              		.p2align 4,,15
   5              		.globl	naiveMult
   7              	naiveMult:
   8              	.LFB18:
   9              		.file 1 "assembler-experiment.c"
   1:assembler-experiment.c **** #include <time.h>
   2:assembler-experiment.c **** #include <sys/time.h>
   3:assembler-experiment.c **** #include <stdio.h>
   4:assembler-experiment.c **** #include <stdlib.h>   
   5:assembler-experiment.c **** 
   6:assembler-experiment.c **** 
   7:assembler-experiment.c **** #define IDX(Y, X) (ndim * Y + X) //rows first
   8:assembler-experiment.c **** 
   9:assembler-experiment.c **** int ndim = 2048;
  10:assembler-experiment.c **** 
  11:assembler-experiment.c **** void  naiveMult(double first[], double second[], double multiplied[])
  12:assembler-experiment.c **** {
  10              		.loc 1 12 0
  11              		.cfi_startproc
  12              	.LBB2:
  13:assembler-experiment.c ****     double sum;
  14:assembler-experiment.c ****     for (int i = 0; i < ndim; i++)
  13              		.loc 1 14 0
  14 0000 448B0500 		movl	ndim(%rip), %r8d
  14      000000
  15 0007 4585C0   		testl	%r8d, %r8d
  16 000a 7E7E     		jle	.L10
  17              	.LBE2:
  12:assembler-experiment.c **** {
  18              		.loc 1 12 0
  19 000c 4155     		pushq	%r13
  20              	.LCFI0:
  21              		.cfi_def_cfa_offset 16
  22              		.cfi_offset 13, -16
  23              	.LBB6:
  11:assembler-experiment.c **** void  naiveMult(double first[], double second[], double multiplied[])
  24              		.loc 1 11 0
  25 000e 418D40FF 		leal	-1(%r8), %eax
  26 0012 4D63C8   		movslq	%r8d, %r9
  27 0015 660F57D2 		xorpd	%xmm2, %xmm2
  28 0019 4989D5   		movq	%rdx, %r13
  29              	.LBE6:
  12:assembler-experiment.c **** {
  30              		.loc 1 12 0
  31 001c 4154     		pushq	%r12
  32              	.LCFI1:
  33              		.cfi_def_cfa_offset 24
  34              		.cfi_offset 12, -24
  35 001e 4989FC   		movq	%rdi, %r12
  36              	.LBB7:
  11:assembler-experiment.c **** void  naiveMult(double first[], double second[], double multiplied[])
  37              		.loc 1 11 0
  38 0021 488D3CC5 		leaq	8(,%rax,8), %rdi
  38      08000000 
GAS LISTING /tmp/cc1fPPOx.s 			page 2


  39 0029 49C1E103 		salq	$3, %r9
  40              	.LBE7:
  12:assembler-experiment.c **** {
  41              		.loc 1 12 0
  42 002d 55       		pushq	%rbp
  43              	.LCFI2:
  44              		.cfi_def_cfa_offset 32
  45              		.cfi_offset 6, -32
  46              		.loc 1 14 0
  47 002e 31ED     		xorl	%ebp, %ebp
  12:assembler-experiment.c **** {
  48              		.loc 1 12 0
  49 0030 53       		pushq	%rbx
  50              	.LCFI3:
  51              		.cfi_def_cfa_offset 40
  52              		.cfi_offset 3, -40
  11:assembler-experiment.c **** void  naiveMult(double first[], double second[], double multiplied[])
  53              		.loc 1 11 0
  54 0031 31DB     		xorl	%ebx, %ebx
  55              	.L3:
  56 0033 4D8D141C 		leaq	(%r12,%rbx), %r10
  57 0037 498D541D 		leaq	0(%r13,%rbx), %rdx
  57      00
  11:assembler-experiment.c **** void  naiveMult(double first[], double second[], double multiplied[])
  58              		.loc 1 11 0 is_stmt 0 discriminator 1
  59 003c 4531DB   		xorl	%r11d, %r11d
  60 003f 90       		.p2align 4,,10
  61              		.p2align 3
  62              	.L6:
  63              	.LBB8:
  64              	.LBB3:
  65 0040 4A8D0C1E 		leaq	(%rsi,%r11), %rcx
  66              	.LBE3:
  67              	.LBE8:
  68 0044 660F28CA 		movapd	%xmm2, %xmm1
  69 0048 31C0     		xorl	%eax, %eax
  70 004a 660F1F44 		.p2align 4,,10
  70      0000
  71              		.p2align 3
  72              	.L4:
  73              	.LBB9:
  74              	.LBB5:
  75              	.LBB4:
  15:assembler-experiment.c ****     {
  16:assembler-experiment.c ****         for (int k = 0; k < ndim; k++)
  17:assembler-experiment.c ****         {
  18:assembler-experiment.c ****             sum = 0;
  19:assembler-experiment.c ****             for (int j = 0; j < ndim; j++)
  20:assembler-experiment.c ****                 sum = sum + first[IDX(i, j)] * second[IDX(j, k)];
  76              		.loc 1 20 0 is_stmt 1 discriminator 2
  77 0050 F2410F10 		movsd	(%r10,%rax,8), %xmm0
  77      04C2
  78 0056 4883C001 		addq	$1, %rax
  79 005a F20F5901 		mulsd	(%rcx), %xmm0
  80 005e 4C01C9   		addq	%r9, %rcx
  19:assembler-experiment.c ****             for (int j = 0; j < ndim; j++)
  81              		.loc 1 19 0 discriminator 2
GAS LISTING /tmp/cc1fPPOx.s 			page 3


  82 0061 4139C0   		cmpl	%eax, %r8d
  83              		.loc 1 20 0 discriminator 2
  84 0064 F20F58C8 		addsd	%xmm0, %xmm1
  19:assembler-experiment.c ****             for (int j = 0; j < ndim; j++)
  85              		.loc 1 19 0 discriminator 2
  86 0068 7FE6     		jg	.L4
  87              	.LBE4:
  21:assembler-experiment.c ****             multiplied[IDX(i, k)] = sum;
  88              		.loc 1 21 0
  89 006a F2420F11 		movsd	%xmm1, (%rdx,%r11)
  89      0C1A
  90 0070 4983C308 		addq	$8, %r11
  16:assembler-experiment.c ****         for (int k = 0; k < ndim; k++)
  91              		.loc 1 16 0
  92 0074 4939FB   		cmpq	%rdi, %r11
  93 0077 75C7     		jne	.L6
  94              	.LBE5:
  14:assembler-experiment.c ****     for (int i = 0; i < ndim; i++)
  95              		.loc 1 14 0
  96 0079 83C501   		addl	$1, %ebp
  97 007c 4C01CB   		addq	%r9, %rbx
  98 007f 4439C5   		cmpl	%r8d, %ebp
  99 0082 75AF     		jne	.L3
 100              	.LBE9:
  22:assembler-experiment.c ****         }
  23:assembler-experiment.c ****     }
  24:assembler-experiment.c **** }
 101              		.loc 1 24 0
 102 0084 5B       		popq	%rbx
 103              	.LCFI4:
 104              		.cfi_restore 3
 105              		.cfi_def_cfa_offset 32
 106 0085 5D       		popq	%rbp
 107              	.LCFI5:
 108              		.cfi_restore 6
 109              		.cfi_def_cfa_offset 24
 110 0086 415C     		popq	%r12
 111              	.LCFI6:
 112              		.cfi_restore 12
 113              		.cfi_def_cfa_offset 16
 114 0088 415D     		popq	%r13
 115              	.LCFI7:
 116              		.cfi_restore 13
 117              		.cfi_def_cfa_offset 8
 118              	.L10:
 119 008a F3       		rep
 120 008b C3       		ret
 121              		.cfi_endproc
 122              	.LFE18:
 124              		.section	.text.startup,"ax",@progbits
 125              		.p2align 4,,15
 126              		.globl	main
 128              	main:
 129              	.LFB19:
  25:assembler-experiment.c **** 
  26:assembler-experiment.c **** 
  27:assembler-experiment.c **** 
GAS LISTING /tmp/cc1fPPOx.s 			page 4


  28:assembler-experiment.c **** int main(int argc, char **argv)
  29:assembler-experiment.c **** {
 130              		.loc 1 29 0
 131              		.cfi_startproc
 132              	.LVL0:
 133 0000 4157     		pushq	%r15
 134              	.LCFI8:
 135              		.cfi_def_cfa_offset 16
 136              		.cfi_offset 15, -16
  30:assembler-experiment.c **** 
  31:assembler-experiment.c **** 
  32:assembler-experiment.c **** 	srand(1);
 137              		.loc 1 32 0
 138 0002 BF010000 		movl	$1, %edi
 138      00
 139              	.LVL1:
  29:assembler-experiment.c **** {
 140              		.loc 1 29 0
 141 0007 4156     		pushq	%r14
 142              	.LCFI9:
 143              		.cfi_def_cfa_offset 24
 144              		.cfi_offset 14, -24
 145 0009 4155     		pushq	%r13
 146              	.LCFI10:
 147              		.cfi_def_cfa_offset 32
 148              		.cfi_offset 13, -32
 149 000b 4154     		pushq	%r12
 150              	.LCFI11:
 151              		.cfi_def_cfa_offset 40
 152              		.cfi_offset 12, -40
 153 000d 55       		pushq	%rbp
 154              	.LCFI12:
 155              		.cfi_def_cfa_offset 48
 156              		.cfi_offset 6, -48
 157 000e 53       		pushq	%rbx
 158              	.LCFI13:
 159              		.cfi_def_cfa_offset 56
 160              		.cfi_offset 3, -56
 161 000f 4883EC18 		subq	$24, %rsp
 162              	.LCFI14:
 163              		.cfi_def_cfa_offset 80
 164              		.loc 1 32 0
 165 0013 E8000000 		call	srand
 165      00
 166              	.LVL2:
  33:assembler-experiment.c ****     double *first = malloc(ndim * ndim * sizeof(double));
 167              		.loc 1 33 0
 168 0018 8B050000 		movl	ndim(%rip), %eax
 168      0000
 169 001e 0FAFC0   		imull	%eax, %eax
 170 0021 4898     		cltq
 171 0023 488D1CC5 		leaq	0(,%rax,8), %rbx
 171      00000000 
 172 002b 4889DF   		movq	%rbx, %rdi
 173 002e E8000000 		call	malloc
 173      00
 174              	.LVL3:
GAS LISTING /tmp/cc1fPPOx.s 			page 5


  34:assembler-experiment.c ****     double *second = malloc(ndim * ndim * sizeof(double));
 175              		.loc 1 34 0
 176 0033 4889DF   		movq	%rbx, %rdi
  33:assembler-experiment.c ****     double *first = malloc(ndim * ndim * sizeof(double));
 177              		.loc 1 33 0
 178 0036 48894424 		movq	%rax, 8(%rsp)
 178      08
 179              	.LVL4:
 180              		.loc 1 34 0
 181 003b E8000000 		call	malloc
 181      00
 182              	.LVL5:
  35:assembler-experiment.c ****     double *multiply = malloc(ndim * ndim * sizeof(double));
 183              		.loc 1 35 0
 184 0040 4889DF   		movq	%rbx, %rdi
  34:assembler-experiment.c ****     double *second = malloc(ndim * ndim * sizeof(double));
 185              		.loc 1 34 0
 186 0043 4989C5   		movq	%rax, %r13
 187              	.LVL6:
 188              		.loc 1 35 0
 189 0046 E8000000 		call	malloc
 189      00
 190              	.LVL7:
  36:assembler-experiment.c ****     int i, c, d;
  37:assembler-experiment.c ****     setbuf(stdout, NULL);
 191              		.loc 1 37 0
 192 004b 488B3D00 		movq	stdout(%rip), %rdi
 192      000000
 193 0052 31F6     		xorl	%esi, %esi
  35:assembler-experiment.c ****     double *multiply = malloc(ndim * ndim * sizeof(double));
 194              		.loc 1 35 0
 195 0054 4989C6   		movq	%rax, %r14
 196              	.LVL8:
 197              		.loc 1 37 0
 198 0057 E8000000 		call	setbuf
 198      00
 199              	.LVL9:
  38:assembler-experiment.c **** 
  39:assembler-experiment.c **** 
  40:assembler-experiment.c ****     for (c = 0 ; c < ndim; c++)
 200              		.loc 1 40 0
 201 005c 8B050000 		movl	ndim(%rip), %eax
 201      0000
 202 0062 85C0     		testl	%eax, %eax
 203 0064 0F8EB700 		jle	.L13
 203      0000
 204 006a 31DB     		xorl	%ebx, %ebx
  41:assembler-experiment.c ****     {
  42:assembler-experiment.c ****         for (d = 0 ; d < ndim; d++)
  43:assembler-experiment.c ****         {
  44:assembler-experiment.c ****             first[IDX(c, d)] = rand() % 10; //int between 0 - 1000
 205              		.loc 1 44 0
 206 006c BD676666 		movl	$1717986919, %ebp
 206      66
 207              	.LVL10:
 208              		.p2align 4,,10
 209 0071 0F1F8000 		.p2align 3
GAS LISTING /tmp/cc1fPPOx.s 			page 6


 209      000000
 210              	.L15:
  40:assembler-experiment.c ****     for (c = 0 ; c < ndim; c++)
 211              		.loc 1 40 0 discriminator 2
 212 0078 4531FF   		xorl	%r15d, %r15d
 213 007b 0F1F4400 		.p2align 4,,10
 213      00
 214              		.p2align 3
 215              	.L14:
 216              		.loc 1 44 0 discriminator 2
 217 0080 0FAFC3   		imull	%ebx, %eax
 218 0083 488B5424 		movq	8(%rsp), %rdx
 218      08
 219 0088 4401F8   		addl	%r15d, %eax
 220 008b 4898     		cltq
 221 008d 4C8D24C2 		leaq	(%rdx,%rax,8), %r12
 222 0091 E8000000 		call	rand
 222      00
 223              	.LVL11:
 224 0096 89C1     		movl	%eax, %ecx
 225 0098 F7ED     		imull	%ebp
 226 009a 89C8     		movl	%ecx, %eax
 227 009c C1F81F   		sarl	$31, %eax
 228 009f C1FA02   		sarl	$2, %edx
 229 00a2 29C2     		subl	%eax, %edx
 230 00a4 8D0492   		leal	(%rdx,%rdx,4), %eax
 231 00a7 01C0     		addl	%eax, %eax
 232 00a9 29C1     		subl	%eax, %ecx
  45:assembler-experiment.c ****             second[IDX(c, d)] = rand() % 10;
 233              		.loc 1 45 0 discriminator 2
 234 00ab 8B050000 		movl	ndim(%rip), %eax
 234      0000
  44:assembler-experiment.c ****             first[IDX(c, d)] = rand() % 10; //int between 0 - 1000
 235              		.loc 1 44 0 discriminator 2
 236 00b1 F20F2AC1 		cvtsi2sd	%ecx, %xmm0
 237              		.loc 1 45 0 discriminator 2
 238 00b5 0FAFC3   		imull	%ebx, %eax
 239 00b8 4401F8   		addl	%r15d, %eax
 240 00bb 4898     		cltq
  44:assembler-experiment.c ****             first[IDX(c, d)] = rand() % 10; //int between 0 - 1000
 241              		.loc 1 44 0 discriminator 2
 242 00bd F2410F11 		movsd	%xmm0, (%r12)
 242      0424
 243              		.loc 1 45 0 discriminator 2
 244 00c3 4D8D64C5 		leaq	0(%r13,%rax,8), %r12
 244      00
 245 00c8 E8000000 		call	rand
 245      00
 246              	.LVL12:
 247 00cd 89C1     		movl	%eax, %ecx
 248 00cf F7ED     		imull	%ebp
 249 00d1 89C8     		movl	%ecx, %eax
 250 00d3 C1F81F   		sarl	$31, %eax
 251 00d6 C1FA02   		sarl	$2, %edx
 252 00d9 29C2     		subl	%eax, %edx
 253 00db 8D0492   		leal	(%rdx,%rdx,4), %eax
 254 00de 01C0     		addl	%eax, %eax
GAS LISTING /tmp/cc1fPPOx.s 			page 7


 255 00e0 29C1     		subl	%eax, %ecx
  46:assembler-experiment.c ****             multiply[IDX(c, d)] = 0;
 256              		.loc 1 46 0 discriminator 2
 257 00e2 8B050000 		movl	ndim(%rip), %eax
 257      0000
  45:assembler-experiment.c ****             second[IDX(c, d)] = rand() % 10;
 258              		.loc 1 45 0 discriminator 2
 259 00e8 F20F2AC1 		cvtsi2sd	%ecx, %xmm0
 260              		.loc 1 46 0 discriminator 2
 261 00ec 89C2     		movl	%eax, %edx
 262 00ee 0FAFD3   		imull	%ebx, %edx
 263 00f1 4401FA   		addl	%r15d, %edx
  42:assembler-experiment.c ****         for (d = 0 ; d < ndim; d++)
 264              		.loc 1 42 0 discriminator 2
 265 00f4 4183C701 		addl	$1, %r15d
 266              	.LVL13:
 267              		.loc 1 46 0 discriminator 2
 268 00f8 4863D2   		movslq	%edx, %rdx
  42:assembler-experiment.c ****         for (d = 0 ; d < ndim; d++)
 269              		.loc 1 42 0 discriminator 2
 270 00fb 4439F8   		cmpl	%r15d, %eax
 271              		.loc 1 46 0 discriminator 2
 272 00fe 49C704D6 		movq	$0, (%r14,%rdx,8)
 272      00000000 
  45:assembler-experiment.c ****             second[IDX(c, d)] = rand() % 10;
 273              		.loc 1 45 0 discriminator 2
 274 0106 F2410F11 		movsd	%xmm0, (%r12)
 274      0424
  42:assembler-experiment.c ****         for (d = 0 ; d < ndim; d++)
 275              		.loc 1 42 0 discriminator 2
 276 010c 0F8F6EFF 		jg	.L14
 276      FFFF
  40:assembler-experiment.c ****     for (c = 0 ; c < ndim; c++)
 277              		.loc 1 40 0
 278 0112 83C301   		addl	$1, %ebx
 279              	.LVL14:
 280 0115 39D8     		cmpl	%ebx, %eax
 281 0117 7E08     		jle	.L13
 282              	.LVL15:
  42:assembler-experiment.c ****         for (d = 0 ; d < ndim; d++)
 283              		.loc 1 42 0 discriminator 1
 284 0119 85C0     		testl	%eax, %eax
 285 011b 0F8F57FF 		jg	.L15
 285      FFFF
 286              	.LVL16:
 287              	.L13:
  47:assembler-experiment.c **** 
  48:assembler-experiment.c ****         }
  49:assembler-experiment.c ****     }
  50:assembler-experiment.c **** 
  51:assembler-experiment.c ****     naiveMult(first,second,multiply);
 288              		.loc 1 51 0
 289 0121 488B7C24 		movq	8(%rsp), %rdi
 289      08
 290 0126 4C89F2   		movq	%r14, %rdx
 291 0129 4C89EE   		movq	%r13, %rsi
 292 012c E8000000 		call	naiveMult
GAS LISTING /tmp/cc1fPPOx.s 			page 8


 292      00
 293              	.LVL17:
  52:assembler-experiment.c **** }
 294              		.loc 1 52 0
 295 0131 4883C418 		addq	$24, %rsp
 296              	.LCFI15:
 297              		.cfi_def_cfa_offset 56
 298 0135 31C0     		xorl	%eax, %eax
 299 0137 5B       		popq	%rbx
 300              	.LCFI16:
 301              		.cfi_def_cfa_offset 48
 302 0138 5D       		popq	%rbp
 303              	.LCFI17:
 304              		.cfi_def_cfa_offset 40
 305 0139 415C     		popq	%r12
 306              	.LCFI18:
 307              		.cfi_def_cfa_offset 32
 308 013b 415D     		popq	%r13
 309              	.LCFI19:
 310              		.cfi_def_cfa_offset 24
 311              	.LVL18:
 312 013d 415E     		popq	%r14
 313              	.LCFI20:
 314              		.cfi_def_cfa_offset 16
 315              	.LVL19:
 316 013f 415F     		popq	%r15
 317              	.LCFI21:
 318              		.cfi_def_cfa_offset 8
 319 0141 C3       		ret
 320              		.cfi_endproc
 321              	.LFE19:
 323              		.globl	ndim
 324              		.data
 325              		.align 16
 328              	ndim:
 329 0000 00080000 		.long	2048
 330              		.text
 331              	.Letext0:
 332              		.file 2 "/usr/lib/gcc/x86_64-linux-gnu/4.7/include/stddef.h"
 333              		.file 3 "/usr/include/x86_64-linux-gnu/bits/types.h"
 334              		.file 4 "/usr/include/stdio.h"
 335              		.file 5 "/usr/include/libio.h"
 336              		.file 6 "/usr/include/stdlib.h"
GAS LISTING /tmp/cc1fPPOx.s 			page 9


DEFINED SYMBOLS
                            *ABS*:0000000000000000 assembler-experiment.c
     /tmp/cc1fPPOx.s:7      .text:0000000000000000 naiveMult
     /tmp/cc1fPPOx.s:328    .data:0000000000000000 ndim
     /tmp/cc1fPPOx.s:128    .text.startup:0000000000000000 main

UNDEFINED SYMBOLS
srand
malloc
stdout
setbuf
rand
