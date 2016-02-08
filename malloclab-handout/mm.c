/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 *
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "ateam",
    /* First member's full name */
    "Casey Hofford",
    /* First member's email address */
    "casey.hofford@coloradocollege.edu",
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email address (leave blank if none) */
    ""
};

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8
#define WSIZE 4
#define DSIZE 8
#define CHUNKSIZE (1<<12)

#define MAX(x, y) ((x) > (y)? (x) : (y))

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)

/*IDK what this really does.... It certainly doesn't work... if I understand correctly size_t is of undefined size so this call could give back an unpredictable #*/
#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

/* Pack a size and allocated bit into a word */
#define PACK(size, alloc) ((size) | (alloc))

/* Read the size and allocated fields from address p */
#define GET_SIZE(p) (GET(p) & ~0x7)
#define GET_ALLOC(p) (GET(p) & 0x1)

/* Read and write a word at address p */
#define GET(p) (*(unsigned int *)(p))
#define PUT(p, val) (*(unsigned int *)(p) = (val))

/* Given block ptr bp, compute address of next and previous blocks */
#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE)))
#define PREV_BLKP(bp) ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE)))

/*#define NEXT_FREE(bp) (GET(bp));
#define PREV_FREE(bp) (GET(bp + WSIZE));*/

/* Given block ptr bp, compute address of its header and footer */
#define HDRP(bp) ((char *)(bp) - WSIZE)
#define FTRP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

static char *heap_listp;
static char *free_start;
//static char *epihead;

static void remove_node(void *bp)
{

  unsigned int next = GET(bp);
  unsigned int prev = GET((char *)(bp)+WSIZE);
  //printf("83-REMOVE_NODE::NEXT:%u PREV:%u\n",next,prev );
  if(prev != 0 && next != 0)
  {
    PUT(prev, next);
    PUT((char *)(next) + WSIZE, prev);
  }
  else if(prev == 0)
  {
    PUT((char *)(next) + WSIZE, 0);
    PUT(free_start, (unsigned int)(next));
  }
  else
  {
    PUT(prev, 0);
  }
}

static void *coalesce(void *bp)
{
  size_t prev_alloc = GET_ALLOC((((char *)(bp) - DSIZE)));
  size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
  size_t size = GET_SIZE(HDRP(bp));
  //printf("p: %d, n: %d, sz: %d\n",prev_alloc,next_alloc,size );

  if (prev_alloc && next_alloc)
  {
    //printf("case 1\n");
    return bp;
  }

  else if (prev_alloc && !next_alloc)
  {
    //printf("case 2: %u\n", (unsigned int)NEXT_BLKP(bp));
    remove_node(bp + GET_SIZE(HDRP(bp)));
    size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));
  }

  else if (!prev_alloc && next_alloc)
  {
    //printf("case 3\n");
    remove_node(bp);
    size += GET_SIZE(HDRP(PREV_BLKP(bp)));
    PUT(FTRP(bp), PACK(size, 0));
    PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
    bp = PREV_BLKP(bp);
  }

  else
  {
    //printf("case 4\n");
    remove_node(NEXT_BLKP(bp));
    remove_node(bp);
    size += GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(FTRP(NEXT_BLKP(bp)));
    PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
    PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
    bp = PREV_BLKP(bp);
  }

  return bp;
}


static void *extend_heap(size_t words)
{
  char *bp;
  size_t size;
  /* Allocate an even number of words to maintain alignment */
  size = (words % 2) ? (words+1) * WSIZE : words * WSIZE;
  if ((long)(bp = mem_sbrk(size)) == -1)
  {
    return NULL;
  }
  /* Initialize free block*/
  //printf("line 155, size of extended heap: %u\n",(unsigned int)(size));
  PUT(HDRP(bp), PACK(size,0));
  PUT(FTRP(bp), PACK(size,0));
  PUT(HDRP(NEXT_BLKP(bp)), PACK(0,1));//epilogue header
  PUT(bp, GET(free_start));
  PUT((char *)(bp) + WSIZE, (unsigned int)(free_start));
  /*if(GET_SIZE(HDRP(free_start)) != 0)
  {
    PUT(free_start + WSIZE, (unsigned int)(bp));
  }*/
  //printf("Line 165: %u\n", (unsigned int)(bp));
  PUT(free_start, (unsigned int)(bp));
  //epihead = HDRP(NEXT_BLKP(bp));
  if(GET(NEXT_BLKP(bp)) != 0 && GET(PREV_BLKP(bp)) != 0)
  {
    void *blockStart = coalesce(bp);
    return blockStart;
  }
  else
  {
    return bp;
  }
}


/*
 * mm_init - initialize the malloc package.
 */
 int mm_init(void)
 {
   /* Create the initial empty heap */
   if ((heap_listp = mem_sbrk(8*WSIZE)) == (void *)-1)
    return -1;
   PUT(heap_listp, 0);
   PUT(heap_listp + (WSIZE), 0);
   PUT(heap_listp + (2*WSIZE), 0);
   /* Alignment padding */
   PUT(heap_listp + (3*WSIZE), PACK(DSIZE, 1)); /* Prologue header */
   //PUT(heap_listp + (2*WSIZE), (unsigned int)(heap_listp + (7*WSIZE)));starting block points to first usable block
   PUT(heap_listp + (4*WSIZE), 0);
   PUT(heap_listp + (5*WSIZE), 0);
   PUT(heap_listp + (6*WSIZE), PACK(DSIZE, 1)); /* Prologue footer */
   PUT(heap_listp + (7*WSIZE), PACK(0, 1));
   /* Epilogue header */
   heap_listp += (6*WSIZE);
   free_start = (heap_listp - DSIZE);
   void *start;
  /* Extend the empty heap with a free block of CHUNKSIZE bytes */
  if ((start = extend_heap(CHUNKSIZE/WSIZE)) == NULL)
  {
    printf("Initial extend heap failure\n");
    return -1;
  }
  //printf("Start: %u\n", (unsigned int)(start));
  //free_start = start;
  //PUT(start, heap_listp + WSIZE)
  return 0;
}

/*
This function takes in the adjusted size of a block (to include header and footer) and then returns a location in the heap where it can fit as a void pointer.
*/
void *find_fit(size_t asize)
{
  void *bp;
  for (bp = (void *)(GET(free_start)); bp > 0; bp = (void *)GET((unsigned int)bp))
  {
    //printf("Free Start = %u\n", (unsigned int)(bp));
    if (asize <= GET_SIZE(HDRP(bp)))
    {
      return bp;
    }
  }
  return NULL; /* No fit */
}

/*
This function takes in an address and a block size and places a header and footer at the appropriate locations in the heap
*/
static void place(void *bp, size_t asize)
{
  size_t csize = GET_SIZE(HDRP(bp));
  if ((csize - asize) >= (2*DSIZE))//when the block to be placed is smaller than the available space
  {
    PUT(HDRP(bp), PACK(asize, 1));
    PUT(FTRP(bp), PACK(asize, 1));
    unsigned int next_pntr = GET(bp);
    //printf("Next: %u\n", next_pntr);
    unsigned int prev_pntr = GET((char *)(bp) + WSIZE);
    //printf("Prev: %u\n", prev_pntr);
    bp = NEXT_BLKP(bp);
    PUT(HDRP(bp), PACK(csize-asize, 0));
    PUT(FTRP(bp), PACK(csize-asize, 0));

    PUT(bp, next_pntr);
    PUT((char *)(bp) +WSIZE, prev_pntr);
    if(prev_pntr != 0)
      PUT(prev_pntr, (unsigned int)(bp));
    else
      PUT(free_start, (unsigned int)(bp));
    if(next_pntr != 0)
      PUT((char *)(next_pntr)+WSIZE, (unsigned int)(bp));
  }
else
  {
    //printf("(place) - block filled\n");
    remove_node(bp);
    PUT(HDRP(bp), PACK(csize, 1));
    PUT(FTRP(bp), PACK(csize, 1));
  }
}

/*
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
  size_t asize;
  /* Adjusted block size */
  size_t extendsize; /* Amount to extend heap if no fit */
  char *bp;
  /* Ignore spurious requests */
  if (size == 0)
    return NULL;
  /* Adjust block size to include overhead and alignment reqs. */
  if (size <= DSIZE)
    asize = 2*DSIZE;
  else
    asize = DSIZE * ((size + (DSIZE) + (DSIZE-1)) / DSIZE);

  /* Search the free list for a fit */
  if ((bp = find_fit(asize)) != NULL)
  {
    //remove links for previously free node
    //printf("(mm_malloc) - sufficient block found\n");
    place(bp, asize);
    return bp;
  }

  /* No fit found. Get more memory and place the block */
  extendsize = MAX(asize,CHUNKSIZE);
  if ((bp = extend_heap(extendsize/WSIZE)) == NULL)
    return NULL;
  place(bp, asize);
  return bp;
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *bp)
{
  size_t size = GET_SIZE(HDRP(bp));
  PUT(HDRP(bp), PACK(size, 0));
  PUT(FTRP(bp), PACK(size, 0));
  PUT(bp, (unsigned int)GET(free_start));
  PUT((char *)(GET(free_start))+WSIZE, (unsigned int)(bp));
  PUT((char *)(bp) + WSIZE, 0);
  PUT(free_start, (unsigned int)bp);
  if(GET(NEXT_BLKP(bp)) != 0 && GET(PREV_BLKP(bp)) != 0)
  {
    coalesce(bp);
  }
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
    void *oldptr = ptr;
    void *newptr;
    size_t copySize;

    newptr = mm_malloc(size);
    if (newptr == NULL)
    {
      printf("Malloc Error, Are any free blocks available?");
      return NULL;
    }
    copySize = *(size_t *)((char *)oldptr - WSIZE);
    if (size < copySize)
      copySize = size;
    memmove(newptr, oldptr, copySize);
    mm_free(oldptr);
    return newptr;
}
