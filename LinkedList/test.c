#include <linked_list.h>
#include <stdlib.h>
#include <stdio.h>

node_p first;
node_p other_first;

int main( int argc, char **argv )
{
    /*fflush(stdout);*/
    first = ll_add_front(first, 4.1);
    first = ll_add_front(first, 7.3);
    first = ll_add_front(first, 3.5);
    first = ll_add_front(first, 85);
    first = ll_add_front(first, 9.5);
    ll_print( first );
    other_first = ll_rev_copy(first);
    ll_print( other_first );
    other_first = ll_add_front(other_first, 8.3);
    other_first = ll_rev(NULL, other_first);
    ll_print( other_first );
    first = ll_add_front(other_first, 7.5);
    first = ll_add_front(other_first, 655);
    ll_print(first);
    first = ll_rev_copy(other_first);
    ll_print(first);
    ll_free(first);
    ll_free(other_first);
    free(first);//despite clearing this here and in the linked_list.c file when I print it still prints the first value followed by a 0, I have to set first to NULL
    free(other_first);
    first = NULL;
    other_first = NULL;
    ll_print(first);
    ll_print(other_first);
    return 0;
}
