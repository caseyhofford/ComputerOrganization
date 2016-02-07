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

/* Given block ptr bp, compute address of its header and footer */
#define HDRP(bp) ((char *)(bp) - WSIZE)
#define FTRP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

static char *heap_listp;

static void *coalesce(void *bp)
{
  size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
  size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
  size_t size = GET_SIZE(HDRP(bp));

    /* Case 1 */
  if (prev_alloc && next_alloc)
  {
    return bp;
  }

    /* Case 2 */
  else if (prev_alloc && !next_alloc)
  {
    size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size,0));
  }

  /* Case 3 */
  else if (!prev_alloc && next_alloc)
  {
    size += GET_SIZE(HDRP(PREV_BLKP(bp)));
    PUT(FTRP(bp), PACK(size, 0));
    PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
    bp = PREV_BLKP(bp);
  }

    /* Case 4 */
  else
  {
    size += GET_SIZE(HDRP(PREV_BLKP(bp))) +
    GET_SIZE(FTRP(NEXT_BLKP(bp)));
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
  PUT(HDRP(bp), PACK(size,0));
  PUT(FTRP(bp), PACK(size,0));
  PUT(HDRP(NEXT_BLKP(bp)), PACK(0,1));
  return coalesce(bp);
}


/*
 * mm_init - initialize the malloc package.
 */
 int mm_init(void)
 {
   /* Create the initial empty heap */
   if ((heap_listp = mem_sbrk(4*WSIZE)) == (void *)-1)
   return -1;
   PUT(heap_listp, 0);
   /* Alignment padding */
   PUT(heap_listp + (1*WSIZE), PACK(DSIZE, 1)); /* Prologue header */
   PUT(heap_listp + (2*WSIZE), PACK(DSIZE, 1)); /* Prologue footer */
   PUT(heap_listp + (3*WSIZE), PACK(0, 1));
   /* Epilogue header */
   heap_listp += (2*WSIZE);
  /* Extend the empty heap with a free block of CHUNKSIZE bytes */
  if (extend_heap(CHUNKSIZE/WSIZE) == NULL)
  {
    return -1;
  }
  return 0;
}

/*
This function takes in the adjusted size of a block (to include header and footer) and then returns a location in the heap where it can fit as a void pointer.
*/
void *find_fit(size_t asize)
{
  void *bp;
  for (bp = heap_listp; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp))
  {
    if (!GET_ALLOC(HDRP(bp)) && (asize <= GET_SIZE(HDRP(bp))))
    {
      return bp;
    }
  }
  return NULL; /* No fit */
}

/*
This function takes in an address and a block size and places a header and footer at the appropriate locations in the heap returning 1 if it was placed and 0 if there was an error
*/
static void place(void *bp, size_t asize)
{
  size_t csize = GET_SIZE(HDRP(bp));
  if ((csize - asize) >= (2*DSIZE))//checks if the current block size will require an unallocated block afterwards
  {
    PUT(HDRP(bp), PACK(asize, 1));
    PUT(FTRP(bp), PACK(asize, 1));
    bp = NEXT_BLKP(bp);
    PUT(HDRP(bp), PACK(csize-asize, 0));
    PUT(FTRP(bp), PACK(csize-asize, 0));
  }
else
  {
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
 * mm_free - Freeing a block fills the header and footer for the requested block with their size and an unallocated flag
 */
void mm_free(void *bp)
{
  size_t size = GET_SIZE(HDRP(bp));
  PUT(HDRP(bp), PACK(size, 0));
  PUT(FTRP(bp), PACK(size, 0));
  coalesce(bp);
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
  void *oldptr = ptr;
  void *newptr;
  if(ptr == NULL)//these next two ifs are for the odd cases where realloc is used to malloc or free, saves ops by checking for case rather than defaulting through rest of method
  {
    newptr = mm_malloc(size);
    return newptr;
  }
  if(size == 0)
  {
    mm_free(oldptr);
  }
  size_t copySize;
  int currSize = GET_SIZE(HDRP(oldptr));
  void *prevBlkP = PREV_BLKP(oldptr);
  void *nextBlkP = NEXT_BLKP(oldptr);
  int nextAlloc = GET_ALLOC(HDRP(nextBlkP));
  int prevAlloc = GET_ALLOC(HDRP(prevBlkP));
  int nextSz = GET_SIZE(HDRP(nextBlkP));
  int prevSz = GET_SIZE(HDRP(prevBlkP));
  copySize = *(size_t *)((char *)oldptr - WSIZE);//copies just the used bytes from the previous block
  if(!prevAlloc && ((prevSz + currSize) >= size))
  {
    memmove(prevBlkP, oldptr, copySize);
    PUT(HDRP(prevBlkP), PACK((prevSz + currSize), 0));//allows place to see this as one large block
    place(prevBlkP, ALIGN(size));
    return prevBlkP;
  }
  else if(!nextAlloc && ((nextSz + currSize) >= size))
  {
    PUT(HDRP(oldptr), PACK((nextSz + currSize), 0));
    place(oldptr, ALIGN(size));
    return oldptr;
  }
  else if(!nextAlloc && !prevAlloc && ((nextSz + currSize + prevSz) >= size))
  {
    PUT(HDRP(prevBlkP), PACK((nextSz + currSize + prevSz), 0));
    memmove(prevBlkP, oldptr, copySize);
    place(prevBlkP, ALIGN(size));
    return prevBlkP;
  }
  else
  {
    newptr = mm_malloc(size);
    if (newptr == NULL)
    {
      printf("Malloc Error, Are any free blocks available?");
      return NULL;
    }
    if (size < copySize)
      copySize = size;
    memmove(newptr, oldptr, copySize);
    mm_free(oldptr);
    return newptr;
  }
}
