struct word_at_a_time {
	const unsigned long one_bits, high_bits;
};     
#define REPEAT_BYTE(x)   ((~0ul / 0xff) * (x))
#define WORD_AT_A_TIME_CONSTANTS { REPEAT_BYTE(0x01), REPEAT_BYTE(0x80) }

#define aligned_byte_mask(n) ((1ul << 8*(n))-1)
#define prep_zero_mask(a, bits, c) (bits)

unsigned long create_zero_mask(unsigned long bits)
{
	bits = (bits - 1) & ~bits;
	return bits >> 7;
}

unsigned long has_zero(unsigned long a, unsigned long *bits, const struct word_at_a_time *c)
{
	unsigned long mask = ((a - c->one_bits) & ~a) & c->high_bits;
	*bits = mask;
	return mask;
}

unsigned long find_zero(unsigned long mask)
{
	unsigned long ret = mask*0x0001020304050608ul >> 56;
	return ret;
}

#define __get_user(x, ptr)	\
({				\
	switch(sizeof(*(ptr))) { \
	case 8: {		\
		(x) = *(ptr);	\
		break;		\
	};			\
	default:		\
		break;		\
	}			\
	0;			\
})
		

/*
 * Do a strnlen, return length of string *with* final '\0'.
 * 'count' is the user-supplied count, while 'max' is the
 * address space maximum.
 *
 * Return 0 for exceptions (which includes hitting the address
 * space maximum), or 'count+1' if hitting the user-supplied
 * maximum count.
 *
 * NOTE! We can sometimes overshoot the user-supplied maximum
 * if it fits in a aligned 'long'. The caller needs to check
 * the return value against "> max".
 */
long do_strnlen_user(const char *src, unsigned long count, unsigned long max)
{
	const struct word_at_a_time constants = WORD_AT_A_TIME_CONSTANTS;
	long align, res = 0;
	unsigned long c;

	if (max > count)
		max = count;

	/*
	 * Do everything aligned. But that means that we
	 * need to also expand the maximum..
	 */
	align = (sizeof(long) - 1) & (unsigned long)src;
	src -= align;
	max += align;

	if ((__get_user(c,(unsigned long *)src)))
		return 0;
	c |= aligned_byte_mask(align);

	for (;;) {
		unsigned long data;
		if (has_zero(c, &data, &constants)) {
			data = prep_zero_mask(c, data, &constants);
			data = create_zero_mask(data);
			return res + find_zero(data) + 1 - align;
		}
		res += sizeof(unsigned long);
		if ((max < sizeof(unsigned long)))
			break;
		max -= sizeof(unsigned long);
		if ((__get_user(c,(unsigned long *)(src+res))))
			return 0;
	}
	res -= align;

	/*
	 * Uhhuh. We hit 'max'. But was that the user-specified maximum
	 * too? If so, return the marker for "too long".
	 */
	if (res >= count)
		return count+1;

	/*
	 * Nope: we hit the address space limit, and we still had more
	 * characters the caller would have wanted. That's 0.
	 */
	return 0;
}

/**
 * strnlen_user: - Get the size of a user string INCLUDING final NUL.
 * @str: The string to measure.
 * @count: Maximum count (including NUL character)
 *
 * Context: User context only.  This function may sleep.
 *
 * Get the size of a NUL-terminated string in user space.
 *
 * Returns the size of the string INCLUDING the terminating NUL.
 * If the string is too long, returns 'count+1'.
 * On exception (or invalid count), returns 0.
 */
long strnlen_user(const char *str, long count)
{
	unsigned long max_addr, src_addr;

	if (count <= 0)
		return 0;

	max_addr = -1UL;
	src_addr = (unsigned long)str;

	if (src_addr < max_addr) {
		unsigned long max = max_addr - src_addr;
		return do_strnlen_user(str, count, max);
	}

	return 0;
}
