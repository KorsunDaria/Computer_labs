
lab3_o3_x86.o:	file format mach-o 32-bit i386

Disassembly of section __TEXT,__text:

00000000 <_CalculateSin>:
       0: 55                           	pushl	%ebp
       1: 89 e5                        	movl	%esp, %ebp
       3: 57                           	pushl	%edi
       4: 56                           	pushl	%esi
       5: 83 ec 08                     	subl	$0x8, %esp
       8: 8b 45 10                     	movl	0x10(%ebp), %eax
       b: f2 0f 10 45 08               	movsd	0x8(%ebp), %xmm0
      10: 83 f8 02                     	cmpl	$0x2, %eax
      13: 0f 8c ae 00 00 00            	jl	0xc7 <_CalculateSin+0xc7>
      19: e8 00 00 00 00               	calll	0x1e <_CalculateSin+0x1e>
      1e: 59                           	popl	%ecx
      1f: 66 0f 28 89 c2 00 00 00      	movapd	0xc2(%ecx), %xmm1
      27: 66 0f 57 c8                  	xorpd	%xmm0, %xmm1
      2b: f2 0f 59 c8                  	mulsd	%xmm0, %xmm1
      2f: 8d 48 ff                     	leal	-0x1(%eax), %ecx
      32: 83 f8 02                     	cmpl	$0x2, %eax
      35: 75 13                        	jne	0x4a <_CalculateSin+0x4a>
      37: b8 04 00 00 00               	movl	$0x4, %eax
      3c: 66 0f 28 d0                  	movapd	%xmm0, %xmm2
      40: f6 c1 01                     	testb	$0x1, %cl
      43: 75 66                        	jne	0xab <_CalculateSin+0xab>
      45: e9 7d 00 00 00               	jmp	0xc7 <_CalculateSin+0xc7>
      4a: 89 ca                        	movl	%ecx, %edx
      4c: 83 e2 fe                     	andl	$-0x2, %edx
      4f: b8 05 00 00 00               	movl	$0x5, %eax
      54: 66 0f 28 d0                  	movapd	%xmm0, %xmm2
      58: 0f 1f 84 00 00 00 00 00      	nopl	(%eax,%eax)
      60: 8d 70 fe                     	leal	-0x2(%eax), %esi
      63: 8d 78 fd                     	leal	-0x3(%eax), %edi
      66: 0f af f7                     	imull	%edi, %esi
      69: 0f 57 db                     	xorps	%xmm3, %xmm3
      6c: f2 0f 2a de                  	cvtsi2sd	%esi, %xmm3
      70: 66 0f 28 e1                  	movapd	%xmm1, %xmm4
      74: f2 0f 5e e3                  	divsd	%xmm3, %xmm4
      78: f2 0f 59 e2                  	mulsd	%xmm2, %xmm4
      7c: f2 0f 58 c4                  	addsd	%xmm4, %xmm0
      80: 8d 70 ff                     	leal	-0x1(%eax), %esi
      83: 0f af f0                     	imull	%eax, %esi
      86: 0f 57 db                     	xorps	%xmm3, %xmm3
      89: f2 0f 2a de                  	cvtsi2sd	%esi, %xmm3
      8d: 66 0f 28 d1                  	movapd	%xmm1, %xmm2
      91: f2 0f 5e d3                  	divsd	%xmm3, %xmm2
      95: f2 0f 59 d4                  	mulsd	%xmm4, %xmm2
      99: f2 0f 58 c2                  	addsd	%xmm2, %xmm0
      9d: 83 c0 04                     	addl	$0x4, %eax
      a0: 83 c2 fe                     	addl	$-0x2, %edx
      a3: 75 bb                        	jne	0x60 <_CalculateSin+0x60>
      a5: 48                           	decl	%eax
      a6: f6 c1 01                     	testb	$0x1, %cl
      a9: 74 1c                        	je	0xc7 <_CalculateSin+0xc7>
      ab: 8d 48 ff                     	leal	-0x1(%eax), %ecx
      ae: 83 c0 fe                     	addl	$-0x2, %eax
      b1: 0f af c1                     	imull	%ecx, %eax
      b4: 0f 57 db                     	xorps	%xmm3, %xmm3
      b7: f2 0f 2a d8                  	cvtsi2sd	%eax, %xmm3
      bb: f2 0f 5e cb                  	divsd	%xmm3, %xmm1
      bf: f2 0f 59 d1                  	mulsd	%xmm1, %xmm2
      c3: f2 0f 58 c2                  	addsd	%xmm2, %xmm0
      c7: f2 0f 11 45 f0               	movsd	%xmm0, -0x10(%ebp)
      cc: dd 45 f0                     	fldl	-0x10(%ebp)
      cf: 83 c4 08                     	addl	$0x8, %esp
      d2: 5e                           	popl	%esi
      d3: 5f                           	popl	%edi
      d4: 5d                           	popl	%ebp
      d5: c3                           	retl
