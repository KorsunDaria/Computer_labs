
lab3_o0_x86.o:	file format mach-o 32-bit i386

Disassembly of section __TEXT,__text:

00000000 <_CalculateSin>:
       0: 55                           	pushl	%ebp
       1: 89 e5                        	movl	%esp, %ebp
       3: 83 ec 28                     	subl	$0x28, %esp
       6: e8 00 00 00 00               	calll	0xb <_CalculateSin+0xb>
       b: 58                           	popl	%eax
       c: 89 45 dc                     	movl	%eax, -0x24(%ebp)
       f: 8b 45 10                     	movl	0x10(%ebp), %eax
      12: f2 0f 10 45 08               	movsd	0x8(%ebp), %xmm0
      17: f2 0f 10 45 08               	movsd	0x8(%ebp), %xmm0
      1c: f2 0f 11 45 f8               	movsd	%xmm0, -0x8(%ebp)
      21: f2 0f 10 45 08               	movsd	0x8(%ebp), %xmm0
      26: f2 0f 11 45 f0               	movsd	%xmm0, -0x10(%ebp)
      2b: c7 45 ec 02 00 00 00         	movl	$0x2, -0x14(%ebp)
      32: 8b 45 ec                     	movl	-0x14(%ebp), %eax
      35: 3b 45 10                     	cmpl	0x10(%ebp), %eax
      38: 7f 56                        	jg	0x90 <_CalculateSin+0x90>
      3a: 8b 45 dc                     	movl	-0x24(%ebp), %eax
      3d: f2 0f 10 45 08               	movsd	0x8(%ebp), %xmm0
      42: f2 0f 59 c0                  	mulsd	%xmm0, %xmm0
      46: 0f 28 88 a5 00 00 00         	movaps	0xa5(%eax), %xmm1
      4d: 66 0f ef c1                  	pxor	%xmm1, %xmm0
      51: 8b 45 ec                     	movl	-0x14(%ebp), %eax
      54: d1 e0                        	shll	%eax
      56: 83 e8 01                     	subl	$0x1, %eax
      59: 8b 4d ec                     	movl	-0x14(%ebp), %ecx
      5c: d1 e1                        	shll	%ecx
      5e: 83 e9 02                     	subl	$0x2, %ecx
      61: 0f af c1                     	imull	%ecx, %eax
      64: f2 0f 2a c8                  	cvtsi2sd	%eax, %xmm1
      68: f2 0f 5e c1                  	divsd	%xmm1, %xmm0
      6c: f2 0f 59 45 f8               	mulsd	-0x8(%ebp), %xmm0
      71: f2 0f 11 45 f8               	movsd	%xmm0, -0x8(%ebp)
      76: f2 0f 10 45 f8               	movsd	-0x8(%ebp), %xmm0
      7b: f2 0f 58 45 f0               	addsd	-0x10(%ebp), %xmm0
      80: f2 0f 11 45 f0               	movsd	%xmm0, -0x10(%ebp)
      85: 8b 45 ec                     	movl	-0x14(%ebp), %eax
      88: 83 c0 01                     	addl	$0x1, %eax
      8b: 89 45 ec                     	movl	%eax, -0x14(%ebp)
      8e: eb a2                        	jmp	0x32 <_CalculateSin+0x32>
      90: f2 0f 10 45 f0               	movsd	-0x10(%ebp), %xmm0
      95: f2 0f 11 45 e0               	movsd	%xmm0, -0x20(%ebp)
      9a: dd 45 e0                     	fldl	-0x20(%ebp)
      9d: 83 c4 28                     	addl	$0x28, %esp
      a0: 5d                           	popl	%ebp
      a1: c3                           	retl
