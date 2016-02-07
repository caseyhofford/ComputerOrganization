void phase_3(char[] input)
{
  int a;
  int b;
  int quant = sscanf(input, "%d %d", a, b);
  if (quant<1)
  {
    explode_bomb();
  }
  switch(a)/*depending on the first int supplied it will look for a different (seemingly) random but specific int as the next variable, if they don't match it will explode. In the code these switch catch statements are stored as an array of locations in the code which contain the check statement with 'a' indicating the index. I suppose this is just how it compiles.*/
  {
  case (0):
    if(b = 365) break;
    else explode_bomb();
  case (1):
    if(b = 410) break;
    else explode_bomb();
  case (2):
    if(b = 675) break;
    else explode_bomb();
  case (3):
    if(b = 97) break;
    else explode_bomb();
  case (4):
    if(b = 450) break;
    else explode_bomb();
  case (5):
    if(b = 357) break;
    else explode_bomb();
  case (6):
    if(b = 905) break;
    else explode_bomb();
  case (7):
    if(b = 448) break;
    else explode_bomb();
  }
}

void phase_4(char[] input)
{
  int a;
  int b;
  int quant = sscanf(input, "%d %d", a, b);
  int receive = func4(a, 0, 14);/*sends this procedure your first value and then 0 and 14*/
  if(b == 35 && receive == 35) return;
  else explode_bomb();
}

int func4(int input, int low, int high)
{
  int ret;
  int half = high/2;
  int mid = half+low;
  if(high>low)/*these calls recursively use the formula here to sum up numbers until the total is 35 and this is returned to phase_4*/
  {
    if((mid) > input)
    {
      int rec = func4(input, low, (mid-1));
      ret = mid + rec;
      return ret;
    }
    else if((mid) < input)
    {
      int comp = (mid)+1;
      int rec = func4(input, comp, high);
      ret = mid + rec;
      return ret;
    }
    else
    {
      return mid;
    }
  }
  else
  {
    return 0;
  }
}


void phase_5(char[] inp)
{
  /*this checks that there is more than 1 argument then does a bitwise AND with 15 (0xf, or 1111[2]) which works as a mask it then goes into a loop that exits when it finds a 15 in an array it is searching through it goes to the index of the number it just received from the array to find the next place to look, all is good and well here as long as this took 15 attempts and the second input is the sum of the elements of the array. The first number I found at the start 622879781 which is set to esi for unclear reasons, maybe just as a hint? It does however work and gives a total of 115 from the array*/
  if(arguments>0)
  {
    int x = argument1;
    int y;
    int total;
    while(x != 15)
    {
      x = array[x];
      y += x;
    }
    if(argument2 = y)
    {
      return;
      /*everybody say yeah!*/
    }
  }
}

phase_6
{
  nodeStruct[] = nodeArray;
  int arcount = 0;
  int rax = array[arcount];
  if(eax <)
  while( r12d < 6)
  {
    r12d++;

    while(rax<=5)
    {
      rbx++;
      if( )
    }
    arcount++;
  }
  while(nodeArray[?].index != 18)
  {
    while()
  }
  while()
  {
    while()
  }
  return
}
/*inputs must all be below 6 and must not repeat*/
/*input 1-5 stored in 0x7fffffffde70-84 in memory sequentially, stack points to the beginning of this array*/
/*0x1000001c3 0x200000385 0x300000255 0x40000032c 0x500000126 0x6000001fa*/
/*0x4b7942486a595062*/
/*451 901 597 812 294 506*/
/*6 5 4 3 2 1 gets through the first set of while loops successfully*/




typedef struct nodeStruct
{
  int value;
  int index;
  struct nodeStruct *next;
} listNode;
