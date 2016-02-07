#
#
#

look:
	ls

compile: linked_list.c
	gcc -I. -c -o linked_list.o linked_list.c

test: test.c linked_list.o
	gcc -I. -c -o test.o test.c
	gcc -o test test.o linked_list.o
	./test

clean:
	rm linked_list.o
	rm test.o
	rm test
