#include <linked_list.h>
#include <stdlib.h>
#include <stdio.h>

node_p first;

int main( int argc, char **argv )
{
    /*fflush(stdout);*/
    node_p n = ll_add_front(first, 4.1);
    first = n;
    n = ll_add_front(first, 7.3);
    first = n;
    ll_print( first );
    first = ll_rev(NULL, first);
    ll_print( first );
    ll_free( first );
    return 0;
}
