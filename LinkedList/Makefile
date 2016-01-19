#
#
#

look:
	ls

linked_list.o: linked_list.c
	gcc -I. -c -o linked_list.o linked_list.c

test.o: test.c
	gcc -I. -c -o test.o test.c

test: linked_list.o test.c
	gcc -o test test.o linked_list.o
