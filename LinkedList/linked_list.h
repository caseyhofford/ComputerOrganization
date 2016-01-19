typedef struct ll_node_struct ll_node;
typedef ll_node *node_p;

struct ll_node_struct
{
    node_p child;
    float data;
};

node_p ll_add_front( node_p t, float x);

node_p ll_rev(node_p);

node_p ll_rev_copy(node_p);

string ll_print(node_p);

void tree_free( node_p t );
