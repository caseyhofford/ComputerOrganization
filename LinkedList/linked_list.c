#include <stdlib.h>
#include <linked_list.h>

node_p first;

tree_p ll_add_front(float x)
{
    node_p added;
    added->data = x;
    if(!first)
    {
        first = added;
        first->child = NULL;
    }
    else
    {
        added->child = first;
        first = added;
    }
}

void tree_free( tree_p t )
{
    if( !t )
    {
    }
    else
    {
        tree_free(t->left);
        tree_free(t->right);
        free(t);
    }
}
