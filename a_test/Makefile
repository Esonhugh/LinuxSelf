CC = aarch64-linux-gnu-gcc
STATIC = -static

# main.c is inline asm with aarch64 asm so CC should be aarch64-linux-gnu-gcc
syscall_test: main.c
	$(CC) -o syscall_test $(STATIC) -no-pie main.c

clean:
	rm syscall_test