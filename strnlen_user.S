#define ALIGN	.align 4
#define ENTRY(n)	\
	.global n;	\
	ALIGN;		\
	n:
#define END(n)		\
	.size n, .-n
#define ENDPROC(n)	\
	.type n, @function;	\
	END(n);

	.text
	.align 5
ENTRY(__strnlen_user)
	mov     x2, x0
1:	subs    x1, x1, #1
	b.mi    2f
	ldrb   w3, [x0], #1
	cbnz    w3, 1b
2:	sub     x0, x0, x2
	ret
ENDPROC(__strnlen_user)
