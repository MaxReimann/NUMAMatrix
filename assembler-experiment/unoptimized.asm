GAS LISTING /tmp/cchZCJz8.s 			page 1


   1              		.file	"assembler-experiment.c"
   2              		.text
   3              	.Ltext0:
   4              		.globl	ndim
   5              		.data
   6              		.align 4
   9              	ndim:
  10 0000 00020000 		.long	512
  11              		.text
  12              		.globl	naiveMult
  14              	naiveMult:
  15              	.LFB0:
  16              		.file 1 "assembler-experiment.c"
   1:assembler-experiment.c **** #include <time.h>
   2:assembler-experiment.c **** #include <sys/time.h>
   3:assembler-experiment.c **** #include <stdio.h>
   4:assembler-experiment.c **** #include <stdlib.h>   
   5:assembler-experiment.c **** 
   6:assembler-experiment.c **** 
   7:assembler-experiment.c **** #define IDX(Y, X) (ndim * Y + X) //rows first
   8:assembler-experiment.c **** 
   9:assembler-experiment.c **** int ndim = 512;
  10:assembler-experiment.c **** 
  11:assembler-experiment.c **** void  naiveMult(double first[], double second[], double multiplied[])
  12:assembler-experiment.c **** {
  17              		.loc 1 12 0
  18              		.cfi_startproc
  19 0000 55       		pushq	%rbp
  20              	.LCFI0:
  21              		.cfi_def_cfa_offset 16
  22              		.cfi_offset 6, -16
  23 0001 4889E5   		movq	%rsp, %rbp
  24              	.LCFI1:
  25              		.cfi_def_cfa_register 6
  26 0004 48897DD8 		movq	%rdi, -40(%rbp)
  27 0008 488975D0 		movq	%rsi, -48(%rbp)
  28 000c 488955C8 		movq	%rdx, -56(%rbp)
  29              	.LBB2:
  13:assembler-experiment.c ****     double sum;
  14:assembler-experiment.c ****     for (int i = 0; i < ndim; i++)
  30              		.loc 1 14 0
  31 0010 C745F400 		movl	$0, -12(%rbp)
  31      000000
  32 0017 E9CB0000 		jmp	.L2
  32      00
  33              	.L7:
  34              	.LBB3:
  15:assembler-experiment.c ****     {
  16:assembler-experiment.c ****         for (int k = 0; k < ndim; k++)
  35              		.loc 1 16 0
  36 001c C745F000 		movl	$0, -16(%rbp)
  36      000000
  37 0023 E9AC0000 		jmp	.L3
  37      00
  38              	.L6:
  17:assembler-experiment.c ****         {
  18:assembler-experiment.c ****             sum = 0;
GAS LISTING /tmp/cchZCJz8.s 			page 2


  39              		.loc 1 18 0
  40 0028 B8000000 		movl	$0, %eax
  40      00
  41 002d 488945F8 		movq	%rax, -8(%rbp)
  42              	.LBB4:
  19:assembler-experiment.c ****             for (int j = 0; j < ndim; j++)
  43              		.loc 1 19 0
  44 0031 C745EC00 		movl	$0, -20(%rbp)
  44      000000
  45 0038 EB62     		jmp	.L4
  46              	.L5:
  20:assembler-experiment.c ****                 sum = sum + first[IDX(i, j)] * second[IDX(j, k)];
  47              		.loc 1 20 0 discriminator 2
  48 003a 8B050000 		movl	ndim(%rip), %eax
  48      0000
  49 0040 89C2     		movl	%eax, %edx
  50 0042 0FAF55F4 		imull	-12(%rbp), %edx
  51 0046 8B45EC   		movl	-20(%rbp), %eax
  52 0049 01D0     		addl	%edx, %eax
  53 004b 4898     		cltq
  54 004d 488D14C5 		leaq	0(,%rax,8), %rdx
  54      00000000 
  55 0055 488B45D8 		movq	-40(%rbp), %rax
  56 0059 4801D0   		addq	%rdx, %rax
  57 005c F20F1008 		movsd	(%rax), %xmm1
  58 0060 8B050000 		movl	ndim(%rip), %eax
  58      0000
  59 0066 89C2     		movl	%eax, %edx
  60 0068 0FAF55EC 		imull	-20(%rbp), %edx
  61 006c 8B45F0   		movl	-16(%rbp), %eax
  62 006f 01D0     		addl	%edx, %eax
  63 0071 4898     		cltq
  64 0073 488D14C5 		leaq	0(,%rax,8), %rdx
  64      00000000 
  65 007b 488B45D0 		movq	-48(%rbp), %rax
  66 007f 4801D0   		addq	%rdx, %rax
  67 0082 F20F1000 		movsd	(%rax), %xmm0
  68 0086 F20F59C1 		mulsd	%xmm1, %xmm0
  69 008a F20F104D 		movsd	-8(%rbp), %xmm1
  69      F8
  70 008f F20F58C1 		addsd	%xmm1, %xmm0
  71 0093 F20F1145 		movsd	%xmm0, -8(%rbp)
  71      F8
  19:assembler-experiment.c ****             for (int j = 0; j < ndim; j++)
  72              		.loc 1 19 0 discriminator 2
  73 0098 8345EC01 		addl	$1, -20(%rbp)
  74              	.L4:
  19:assembler-experiment.c ****             for (int j = 0; j < ndim; j++)
  75              		.loc 1 19 0 is_stmt 0 discriminator 1
  76 009c 8B050000 		movl	ndim(%rip), %eax
  76      0000
  77 00a2 3945EC   		cmpl	%eax, -20(%rbp)
  78 00a5 7C93     		jl	.L5
  79              	.LBE4:
  21:assembler-experiment.c ****             multiplied[IDX(i, k)] = sum;
  80              		.loc 1 21 0 is_stmt 1
  81 00a7 8B050000 		movl	ndim(%rip), %eax
GAS LISTING /tmp/cchZCJz8.s 			page 3


  81      0000
  82 00ad 89C2     		movl	%eax, %edx
  83 00af 0FAF55F4 		imull	-12(%rbp), %edx
  84 00b3 8B45F0   		movl	-16(%rbp), %eax
  85 00b6 01D0     		addl	%edx, %eax
  86 00b8 4898     		cltq
  87 00ba 488D14C5 		leaq	0(,%rax,8), %rdx
  87      00000000 
  88 00c2 488B45C8 		movq	-56(%rbp), %rax
  89 00c6 4801C2   		addq	%rax, %rdx
  90 00c9 488B45F8 		movq	-8(%rbp), %rax
  91 00cd 488902   		movq	%rax, (%rdx)
  16:assembler-experiment.c ****         for (int k = 0; k < ndim; k++)
  92              		.loc 1 16 0
  93 00d0 8345F001 		addl	$1, -16(%rbp)
  94              	.L3:
  16:assembler-experiment.c ****         for (int k = 0; k < ndim; k++)
  95              		.loc 1 16 0 is_stmt 0 discriminator 1
  96 00d4 8B050000 		movl	ndim(%rip), %eax
  96      0000
  97 00da 3945F0   		cmpl	%eax, -16(%rbp)
  98 00dd 0F8C45FF 		jl	.L6
  98      FFFF
  99              	.LBE3:
  14:assembler-experiment.c ****     for (int i = 0; i < ndim; i++)
 100              		.loc 1 14 0 is_stmt 1
 101 00e3 8345F401 		addl	$1, -12(%rbp)
 102              	.L2:
  14:assembler-experiment.c ****     for (int i = 0; i < ndim; i++)
 103              		.loc 1 14 0 is_stmt 0 discriminator 1
 104 00e7 8B050000 		movl	ndim(%rip), %eax
 104      0000
 105 00ed 3945F4   		cmpl	%eax, -12(%rbp)
 106 00f0 0F8C26FF 		jl	.L7
 106      FFFF
 107              	.LBE2:
  22:assembler-experiment.c ****         }
  23:assembler-experiment.c ****     }
  24:assembler-experiment.c **** }
 108              		.loc 1 24 0 is_stmt 1
 109 00f6 5D       		popq	%rbp
 110              	.LCFI2:
 111              		.cfi_def_cfa 7, 8
 112 00f7 C3       		ret
 113              		.cfi_endproc
 114              	.LFE0:
 116              		.globl	main
 118              	main:
 119              	.LFB1:
  25:assembler-experiment.c **** 
  26:assembler-experiment.c **** 
  27:assembler-experiment.c **** 
  28:assembler-experiment.c **** int main(int argc, char **argv)
  29:assembler-experiment.c **** {
 120              		.loc 1 29 0
 121              		.cfi_startproc
 122 00f8 55       		pushq	%rbp
GAS LISTING /tmp/cchZCJz8.s 			page 4


 123              	.LCFI3:
 124              		.cfi_def_cfa_offset 16
 125              		.cfi_offset 6, -16
 126 00f9 4889E5   		movq	%rsp, %rbp
 127              	.LCFI4:
 128              		.cfi_def_cfa_register 6
 129 00fc 53       		pushq	%rbx
 130 00fd 4883EC38 		subq	$56, %rsp
 131              		.cfi_offset 3, -24
 132 0101 897DCC   		movl	%edi, -52(%rbp)
 133 0104 488975C0 		movq	%rsi, -64(%rbp)
  30:assembler-experiment.c **** 
  31:assembler-experiment.c **** 
  32:assembler-experiment.c **** 	srand(1);
 134              		.loc 1 32 0
 135 0108 BF010000 		movl	$1, %edi
 135      00
 136 010d E8000000 		call	srand
 136      00
  33:assembler-experiment.c ****     double *first = malloc(ndim * ndim * sizeof(double));
 137              		.loc 1 33 0
 138 0112 8B150000 		movl	ndim(%rip), %edx
 138      0000
 139 0118 8B050000 		movl	ndim(%rip), %eax
 139      0000
 140 011e 0FAFC2   		imull	%edx, %eax
 141 0121 4898     		cltq
 142 0123 48C1E003 		salq	$3, %rax
 143 0127 4889C7   		movq	%rax, %rdi
 144 012a E8000000 		call	malloc
 144      00
 145 012f 488945E0 		movq	%rax, -32(%rbp)
  34:assembler-experiment.c ****     double *second = malloc(ndim * ndim * sizeof(double));
 146              		.loc 1 34 0
 147 0133 8B150000 		movl	ndim(%rip), %edx
 147      0000
 148 0139 8B050000 		movl	ndim(%rip), %eax
 148      0000
 149 013f 0FAFC2   		imull	%edx, %eax
 150 0142 4898     		cltq
 151 0144 48C1E003 		salq	$3, %rax
 152 0148 4889C7   		movq	%rax, %rdi
 153 014b E8000000 		call	malloc
 153      00
 154 0150 488945D8 		movq	%rax, -40(%rbp)
  35:assembler-experiment.c ****     double *multiply = malloc(ndim * ndim * sizeof(double));
 155              		.loc 1 35 0
 156 0154 8B150000 		movl	ndim(%rip), %edx
 156      0000
 157 015a 8B050000 		movl	ndim(%rip), %eax
 157      0000
 158 0160 0FAFC2   		imull	%edx, %eax
 159 0163 4898     		cltq
 160 0165 48C1E003 		salq	$3, %rax
 161 0169 4889C7   		movq	%rax, %rdi
 162 016c E8000000 		call	malloc
 162      00
GAS LISTING /tmp/cchZCJz8.s 			page 5


 163 0171 488945D0 		movq	%rax, -48(%rbp)
  36:assembler-experiment.c ****     int i, c, d;
  37:assembler-experiment.c ****     setbuf(stdout, NULL);
 164              		.loc 1 37 0
 165 0175 488B0500 		movq	stdout(%rip), %rax
 165      000000
 166 017c BE000000 		movl	$0, %esi
 166      00
 167 0181 4889C7   		movq	%rax, %rdi
 168 0184 E8000000 		call	setbuf
 168      00
  38:assembler-experiment.c **** 
  39:assembler-experiment.c **** 
  40:assembler-experiment.c ****     for (c = 0 ; c < ndim; c++)
 169              		.loc 1 40 0
 170 0189 C745EC00 		movl	$0, -20(%rbp)
 170      000000
 171 0190 E9F10000 		jmp	.L9
 171      00
 172              	.L12:
  41:assembler-experiment.c ****     {
  42:assembler-experiment.c ****         for (d = 0 ; d < ndim; d++)
 173              		.loc 1 42 0
 174 0195 C745E800 		movl	$0, -24(%rbp)
 174      000000
 175 019c E9D20000 		jmp	.L10
 175      00
 176              	.L11:
  43:assembler-experiment.c ****         {
  44:assembler-experiment.c ****             first[IDX(c, d)] = rand() % 10; //int between 0 - 1000
 177              		.loc 1 44 0 discriminator 2
 178 01a1 8B050000 		movl	ndim(%rip), %eax
 178      0000
 179 01a7 89C2     		movl	%eax, %edx
 180 01a9 0FAF55EC 		imull	-20(%rbp), %edx
 181 01ad 8B45E8   		movl	-24(%rbp), %eax
 182 01b0 01D0     		addl	%edx, %eax
 183 01b2 4898     		cltq
 184 01b4 488D14C5 		leaq	0(,%rax,8), %rdx
 184      00000000 
 185 01bc 488B45E0 		movq	-32(%rbp), %rax
 186 01c0 488D1C02 		leaq	(%rdx,%rax), %rbx
 187 01c4 E8000000 		call	rand
 187      00
 188 01c9 89C1     		movl	%eax, %ecx
 189 01cb BA676666 		movl	$1717986919, %edx
 189      66
 190 01d0 89C8     		movl	%ecx, %eax
 191 01d2 F7EA     		imull	%edx
 192 01d4 C1FA02   		sarl	$2, %edx
 193 01d7 89C8     		movl	%ecx, %eax
 194 01d9 C1F81F   		sarl	$31, %eax
 195 01dc 29C2     		subl	%eax, %edx
 196 01de 89D0     		movl	%edx, %eax
 197 01e0 C1E002   		sall	$2, %eax
 198 01e3 01D0     		addl	%edx, %eax
 199 01e5 01C0     		addl	%eax, %eax
GAS LISTING /tmp/cchZCJz8.s 			page 6


 200 01e7 89CA     		movl	%ecx, %edx
 201 01e9 29C2     		subl	%eax, %edx
 202 01eb F20F2AC2 		cvtsi2sd	%edx, %xmm0
 203 01ef F20F1103 		movsd	%xmm0, (%rbx)
  45:assembler-experiment.c ****             second[IDX(c, d)] = rand() % 10;
 204              		.loc 1 45 0 discriminator 2
 205 01f3 8B050000 		movl	ndim(%rip), %eax
 205      0000
 206 01f9 89C2     		movl	%eax, %edx
 207 01fb 0FAF55EC 		imull	-20(%rbp), %edx
 208 01ff 8B45E8   		movl	-24(%rbp), %eax
 209 0202 01D0     		addl	%edx, %eax
 210 0204 4898     		cltq
 211 0206 488D14C5 		leaq	0(,%rax,8), %rdx
 211      00000000 
 212 020e 488B45D8 		movq	-40(%rbp), %rax
 213 0212 488D1C02 		leaq	(%rdx,%rax), %rbx
 214 0216 E8000000 		call	rand
 214      00
 215 021b 89C1     		movl	%eax, %ecx
 216 021d BA676666 		movl	$1717986919, %edx
 216      66
 217 0222 89C8     		movl	%ecx, %eax
 218 0224 F7EA     		imull	%edx
 219 0226 C1FA02   		sarl	$2, %edx
 220 0229 89C8     		movl	%ecx, %eax
 221 022b C1F81F   		sarl	$31, %eax
 222 022e 29C2     		subl	%eax, %edx
 223 0230 89D0     		movl	%edx, %eax
 224 0232 C1E002   		sall	$2, %eax
 225 0235 01D0     		addl	%edx, %eax
 226 0237 01C0     		addl	%eax, %eax
 227 0239 89CA     		movl	%ecx, %edx
 228 023b 29C2     		subl	%eax, %edx
 229 023d F20F2AC2 		cvtsi2sd	%edx, %xmm0
 230 0241 F20F1103 		movsd	%xmm0, (%rbx)
  46:assembler-experiment.c ****             multiply[IDX(c, d)] = 0;
 231              		.loc 1 46 0 discriminator 2
 232 0245 8B050000 		movl	ndim(%rip), %eax
 232      0000
 233 024b 89C2     		movl	%eax, %edx
 234 024d 0FAF55EC 		imull	-20(%rbp), %edx
 235 0251 8B45E8   		movl	-24(%rbp), %eax
 236 0254 01D0     		addl	%edx, %eax
 237 0256 4898     		cltq
 238 0258 488D14C5 		leaq	0(,%rax,8), %rdx
 238      00000000 
 239 0260 488B45D0 		movq	-48(%rbp), %rax
 240 0264 4801C2   		addq	%rax, %rdx
 241 0267 B8000000 		movl	$0, %eax
 241      00
 242 026c 488902   		movq	%rax, (%rdx)
  42:assembler-experiment.c ****         for (d = 0 ; d < ndim; d++)
 243              		.loc 1 42 0 discriminator 2
 244 026f 8345E801 		addl	$1, -24(%rbp)
 245              	.L10:
  42:assembler-experiment.c ****         for (d = 0 ; d < ndim; d++)
GAS LISTING /tmp/cchZCJz8.s 			page 7


 246              		.loc 1 42 0 is_stmt 0 discriminator 1
 247 0273 8B050000 		movl	ndim(%rip), %eax
 247      0000
 248 0279 3945E8   		cmpl	%eax, -24(%rbp)
 249 027c 0F8C1FFF 		jl	.L11
 249      FFFF
  40:assembler-experiment.c ****     for (c = 0 ; c < ndim; c++)
 250              		.loc 1 40 0 is_stmt 1
 251 0282 8345EC01 		addl	$1, -20(%rbp)
 252              	.L9:
  40:assembler-experiment.c ****     for (c = 0 ; c < ndim; c++)
 253              		.loc 1 40 0 is_stmt 0 discriminator 1
 254 0286 8B050000 		movl	ndim(%rip), %eax
 254      0000
 255 028c 3945EC   		cmpl	%eax, -20(%rbp)
 256 028f 0F8C00FF 		jl	.L12
 256      FFFF
  47:assembler-experiment.c **** 
  48:assembler-experiment.c ****         }
  49:assembler-experiment.c ****     }
  50:assembler-experiment.c **** 
  51:assembler-experiment.c ****     naiveMult(first,second,multiply);
 257              		.loc 1 51 0 is_stmt 1
 258 0295 488B55D0 		movq	-48(%rbp), %rdx
 259 0299 488B4DD8 		movq	-40(%rbp), %rcx
 260 029d 488B45E0 		movq	-32(%rbp), %rax
 261 02a1 4889CE   		movq	%rcx, %rsi
 262 02a4 4889C7   		movq	%rax, %rdi
 263 02a7 E8000000 		call	naiveMult
 263      00
 264 02ac B8000000 		movl	$0, %eax
 264      00
  52:assembler-experiment.c **** }
 265              		.loc 1 52 0
 266 02b1 4883C438 		addq	$56, %rsp
 267 02b5 5B       		popq	%rbx
 268 02b6 5D       		popq	%rbp
 269              	.LCFI5:
 270              		.cfi_def_cfa 7, 8
 271 02b7 C3       		ret
 272              		.cfi_endproc
 273              	.LFE1:
 275              	.Letext0:
 276              		.file 2 "/usr/lib/gcc/x86_64-linux-gnu/4.7/include/stddef.h"
 277              		.file 3 "/usr/include/x86_64-linux-gnu/bits/types.h"
 278              		.file 4 "/usr/include/libio.h"
 279              		.file 5 "/usr/include/stdio.h"
GAS LISTING /tmp/cchZCJz8.s 			page 8


DEFINED SYMBOLS
                            *ABS*:0000000000000000 assembler-experiment.c
     /tmp/cchZCJz8.s:9      .data:0000000000000000 ndim
     /tmp/cchZCJz8.s:14     .text:0000000000000000 naiveMult
     /tmp/cchZCJz8.s:118    .text:00000000000000f8 main

UNDEFINED SYMBOLS
srand
malloc
stdout
setbuf
rand