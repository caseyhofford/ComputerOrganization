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
    other_first = ll_add_front(other_first, 8.3);
    ll_print( other_first );
    ll_print(first);
    ll_free( first );
    ll_free(other_first);
    free(first);
    free(other_first);
    return 0;
}
