#include <stdio.h>
#include <string.h>

extern unsigned long __strnlen_user(const char *s, long n);
extern unsigned long strnlen_user(const char *s, long n);

const char foo[] = "HELLO WORLD!";

unsigned long fixed__strnlen_user(const char *s, long n)
{
	unsigned long res = 0;

	res = __strnlen_user(s, n);	

	if (n && res >= n)
		return n+1;

	return res;
}

int main(int argc, char *argv[])
{
	printf("=== string.h ===\n");
	printf("strlen = %d\n", strlen(foo));
	printf("strnlen = %d\n", strnlen(foo, 128));

	printf("=== count = 0 ===\n");
	printf("strnlen_user = %d\n", strnlen_user(foo, 0));
	printf("__strnlen_user = %d\n", __strnlen_user(foo, 0));
	printf("fixed__strnlen_user = %d\n", fixed__strnlen_user(foo, 0));

	printf("=== count < strlen ===\n");
	printf("strnlen_user = %d\n", strnlen_user(foo, 6));
	printf("__strnlen_user = %d\n", __strnlen_user(foo, 6));
	printf("fixed__strnlen_user = %d\n", fixed__strnlen_user(foo, 6));

	printf("=== count = strlen ===\n");
	printf("strnlen_user = %d\n", strnlen_user(foo, 12));
	printf("__strnlen_user = %d\n", __strnlen_user(foo, 12));
	printf("fixed__strnlen_user = %d\n", fixed__strnlen_user(foo, 12));

	printf("=== count > strlen ===\n");
	printf("strnlen_user = %d\n", strnlen_user(foo, 24));
	printf("__strnlen_user = %d\n", __strnlen_user(foo, 24));
	printf("fixed__strnlen_user = %d\n", fixed__strnlen_user(foo, 24));

	return 0;
}
