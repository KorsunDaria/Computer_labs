
lab3_o3_x86-64.o:	file format mach-o 64-bit x86-64

Disassembly of section __TEXT,__text:

0000000000000000 <_CalculateSin>:
       0: 48 83 ff 02                  	cmpq	$0x2, %rdi
       4: 0f 8c bd 00 00 00            	jl	0xc7 <_CalculateSin+0xc7>
       a: 55                           	pushq	%rbp
       b: 48 89 e5                     	movq	%rsp, %rbp
       e: 66 0f 28 0d ba 00 00 00      	movapd	0xba(%rip), %xmm1       ## 0xd0 <_CalculateSin+0xd0>
      16: 66 0f 57 c8                  	xorpd	%xmm0, %xmm1
      1a: f2 0f 59 c8                  	mulsd	%xmm0, %xmm1
      1e: 48 8d 4f ff                  	leaq	-0x1(%rdi), %rcx
      22: 48 83 ff 02                  	cmpq	$0x2, %rdi
      26: 75 13                        	jne	0x3b <_CalculateSin+0x3b>
      28: b8 04 00 00 00               	movl	$0x4, %eax
      2d: 66 0f 28 d0                  	movapd	%xmm0, %xmm2
      31: f6 c1 01                     	testb	$0x1, %cl
      34: 75 70                        	jne	0xa6 <_CalculateSin+0xa6>
      36: e9 8b 00 00 00               	jmp	0xc6 <_CalculateSin+0xc6>
      3b: 48 89 ca                     	movq	%rcx, %rdx
      3e: 48 83 e2 fe                  	andq	$-0x2, %rdx
      42: b8 05 00 00 00               	movl	$0x5, %eax
      47: 66 0f 28 d0                  	movapd	%xmm0, %xmm2
      4b: 0f 1f 44 00 00               	nopl	(%rax,%rax)
      50: 48 8d 70 fe                  	leaq	-0x2(%rax), %rsi
      54: 48 8d 78 fd                  	leaq	-0x3(%rax), %rdi
      58: 48 0f af f7                  	imulq	%rdi, %rsi
      5c: 0f 57 db                     	xorps	%xmm3, %xmm3
      5f: f2 48 0f 2a de               	cvtsi2sd	%rsi, %xmm3
      64: 66 0f 28 e1                  	movapd	%xmm1, %xmm4
      68: f2 0f 5e e3                  	divsd	%xmm3, %xmm4
      6c: f2 0f 59 e2                  	mulsd	%xmm2, %xmm4
      70: f2 0f 58 c4                  	addsd	%xmm4, %xmm0
      74: 48 8d 70 ff                  	leaq	-0x1(%rax), %rsi
      78: 48 0f af f0                  	imulq	%rax, %rsi
      7c: 0f 57 db                     	xorps	%xmm3, %xmm3
      7f: f2 48 0f 2a de               	cvtsi2sd	%rsi, %xmm3
      84: 66 0f 28 d1                  	movapd	%xmm1, %xmm2
      88: f2 0f 5e d3                  	divsd	%xmm3, %xmm2
      8c: f2 0f 59 d4                  	mulsd	%xmm4, %xmm2
      90: f2 0f 58 c2                  	addsd	%xmm2, %xmm0
      94: 48 83 c0 04                  	addq	$0x4, %rax
      98: 48 83 c2 fe                  	addq	$-0x2, %rdx
      9c: 75 b2                        	jne	0x50 <_CalculateSin+0x50>
      9e: 48 ff c8                     	decq	%rax
      a1: f6 c1 01                     	testb	$0x1, %cl
      a4: 74 20                        	je	0xc6 <_CalculateSin+0xc6>
      a6: 48 8d 48 ff                  	leaq	-0x1(%rax), %rcx
      aa: 48 83 c0 fe                  	addq	$-0x2, %rax
      ae: 48 0f af c1                  	imulq	%rcx, %rax
      b2: 0f 57 db                     	xorps	%xmm3, %xmm3
      b5: f2 48 0f 2a d8               	cvtsi2sd	%rax, %xmm3
      ba: f2 0f 5e cb                  	divsd	%xmm3, %xmm1
      be: f2 0f 59 d1                  	mulsd	%xmm1, %xmm2
      c2: f2 0f 58 c2                  	addsd	%xmm2, %xmm0
      c6: 5d                           	popq	%rbp
      c7: c3                           	retq
