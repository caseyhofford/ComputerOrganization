#include <stdlib.h>
#include <stdio.h>
#include <linked_list.h>

node_p ll_add_front(node_p first, float x)
{
    node_p added = (node_p)malloc(sizeof(*first));
    added->data = x;
    if(!first)
    {
      return added;
    }
    else
    {
      added->child = first;
      return added;
    }
    return first;
}

void ll_print(node_p selected)
{
  if(!selected)
  {
    printf("[empty list]\n");
  }
  else
  {
    printf("%f", selected->data);
    if(selected->child != NULL)
    {
      printf(" -> ");
      ll_print(selected->child);
    }
    else
    {
      printf(" -|\n");
    }
  }
}

node_p ll_rev(node_p previous, node_p selected)
{
  if(!selected->child)
  {
    selected->child = previous;
    return selected;
  }
  node_p first = ll_rev(selected, selected->child);
  selected->child = previous;
  return first;
}

node_p ll_rev_copy(node_p old)
{
  node_p first = ll_add_front(NULL, old->data);
  while(old->child)
  {
    first = ll_add_front(first, old->child->data);
    old = old->child;
  }
  return first;
  /*ll_add_front()
  node_p first = ll_rev_copy(selected->child, newrev);
  */
}

void ll_free( node_p selected )
{
  if(!selected)
  {
  }
  else
  {
    ll_free(selected->child);
    free(selected);
    /*printf("%f\n", selected->data);
    printf("Freed");*/
  }
  selected = NULL;
}
