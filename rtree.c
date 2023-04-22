#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>


#define DIMS 2
#define MAX_ENTRIES 4
#define MIN_ENTRIES 2


typedef struct rect{
    int min[DIMS];
    int max[DIMS];
    NODE *child;

} RECT;


typedef struct node{
    int count;
    RECT entries[MAX_ENTRIES];
} NODE;


typedef struct rtree{
    NODE *root;
} RTREE;