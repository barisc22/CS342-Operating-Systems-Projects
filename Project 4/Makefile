obj-m += hello.o
all:	test
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules

test: test.c
	gcc -o test test.c

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean
	rm -fr test test.o *~
