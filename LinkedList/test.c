#include <linked_list.h>
#include <stdlib.h>

int main( int argc, char **argv )
{
    tree_p t = tree_insert(NULL, 4.1);
    tree_free( t );
    return 0;
}
