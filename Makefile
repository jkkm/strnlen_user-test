all: test

test: generic-strnlen_user.o strnlen_user.o

clean:
	rm -f generic-strnlen_user.o strnlen_user.o test
