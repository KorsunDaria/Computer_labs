
lab3_o0_x86-64.o:	file format mach-o 64-bit x86-64

Disassembly of section __TEXT,__text:

0000000000000000 <_CalculateSin>:
       0: 55                           	pushq	%rbp                    // Сохраняем предыдущий базовый указатель
       1: 48 89 e5                     	movq	%rsp, %rbp              // Устанавливаем новый базовый указатель
       4: f2 0f 11 45 f8               	movsd	%xmm0, -0x8(%rbp)       // Сохраняем x (1-й аргумент) в стек
       9: 48 89 7d f0                  	movq	%rdi, -0x10(%rbp)       // Сохраняем N (2-й аргумент) в стек
       d: f2 0f 10 45 f8               	movsd	-0x8(%rbp), %xmm0       // x в xmm0
      12: f2 0f 11 45 e8               	movsd	%xmm0, -0x18(%rbp)      // Инициализируем term = x
      17: f2 0f 10 45 f8               	movsd	-0x8(%rbp), %xmm0       // x в xmm0
      1c: f2 0f 11 45 e0               	movsd	%xmm0, -0x20(%rbp)      // Инициализируем result = x
      21: 48 c7 45 d8 02 00 00 00      	movq	$0x2, -0x28(%rbp)       // Инициализируем n = 2
      29: 48 8b 45 d8                  	movq	-0x28(%rbp), %rax       //  n в rax
      2d: 48 3b 45 f0                  	cmpq	-0x10(%rbp), %rax       // Сравниваем n и N
      31: 7f 6b                        	jg	0x9e <_CalculateSin+0x9e> // Если n > N, выходим из цикла
      33: f2 0f 10 45 f8               	movsd	-0x8(%rbp), %xmm0       //  x
      38: f2 0f 59 45 f8               	mulsd	-0x8(%rbp), %xmm0       // Вычисляем x*x
      3d: 66 48 0f 7e c0               	movq	%xmm0, %rax             // результат в rax
      42: 48 b9 00 00 00 00 00 00 00 80	movabsq	$-0x8000000000000000, %rcx // Загружаем маску для смены знака
      4c: 48 31 c8                     	xorq	%rcx, %rax              // Меняем знак (x*x становится -(x*x))
      4f: 66 48 0f 6e c0               	movq	%rax, %xmm0             // Возвращаем результат в xmm0
      54: 48 8b 45 d8                  	movq	-0x28(%rbp), %rax       // Загружаем n
      58: 48 d1 e0                     	shlq	%rax                    // n*2 (2*n)
      5b: 48 83 e8 01                  	subq	$0x1, %rax              // 2*n - 1
      5f: 48 8b 4d d8                  	movq	-0x28(%rbp), %rcx       // Загружаем n
      63: 48 d1 e1                     	shlq	%rcx                    // n*2 (2*n)
      66: 48 83 e9 02                  	subq	$0x2, %rcx              // 2*n - 2
      6a: 48 0f af c1                  	imulq	%rcx, %rax              // (2*n-1)*(2*n-2)
      6e: f2 48 0f 2a c8               	cvtsi2sd	%rax, %xmm1      // Конвертируем в double
      73: f2 0f 5e c1                  	divsd	%xmm1, %xmm0            // -(x*x)/((2*n-1)*(2*n-2))
      77: f2 0f 59 45 e8               	mulsd	-0x18(%rbp), %xmm0      // term *= результат
      7c: f2 0f 11 45 e8               	movsd	%xmm0, -0x18(%rbp)      // Сохраняем term
      81: f2 0f 10 45 e8               	movsd	-0x18(%rbp), %xmm0      // Загружаем term
      86: f2 0f 58 45 e0               	addsd	-0x20(%rbp), %xmm0      // result += term
      8b: f2 0f 11 45 e0               	movsd	%xmm0, -0x20(%rbp)      // Сохраняем result
      90: 48 8b 45 d8                  	movq	-0x28(%rbp), %rax       // Загружаем n
      94: 48 83 c0 01                  	addq	$0x1, %rax              // n++
      98: 48 89 45 d8                  	movq	%rax, -0x28(%rbp)       // Сохраняем n
      9c: eb 8b                        	jmp	0x29 <_CalculateSin+0x29> // Возврат к началу цикла
      9e: f2 0f 10 45 e0               	movsd	-0x20(%rbp), %xmm0      // Загружаем результат
      a3: 5d                           	popq	%rbp                    // Восстанавливаем базовый указатель
      a4: c3                           	retq                           // Возврат
