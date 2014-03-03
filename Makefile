all:
	gcc -c checksum.c math_alt.c
	gcc -o checksum checksum.o math_alt.o -lpthread -lm
clean:
	rm checksum checksum.o math_alt.o
