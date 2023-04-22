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
    bool is_leaf;
    RECT entries[MAX_ENTRIES];
} NODE;


typedef struct rtree{
    NODE *root;
} RTREE;


RTREE* create_rtree(){
    RTREE *rtree = (RTREE*)malloc(sizeof(RTREE));
    rtree->root = (NODE*)malloc(sizeof(NODE));
    rtree->root->count = 0;
    rtree->root->is_leaf = true;
    return rtree;
}

void insert_into_rtree(RTREE *rtree, RECT *rect){
    NODE *root = rtree->root;
    if(root->count < MAX_ENTRIES){
        root->entries[root->count] = *rect;
        root->count++;
    }
    else{
        NODE *new_root = (NODE*)malloc(sizeof(NODE));
        new_root->count = 1;
        new_root->is_leaf = false;
        new_root->entries[0].child = root;
        split_node(new_root, 0);
        rtree->root = new_root;
        insert_into_node(rtree->root, rect);
    }
}

void split_node(NODE *node, int index){
    NODE *new_node = (NODE*)malloc(sizeof(NODE));
    new_node->count = MIN_ENTRIES;
    new_node->is_leaf = node->entries[index].child->is_leaf;
    for(int i = 0; i < MIN_ENTRIES; i++){
        new_node->entries[i] = node->entries[index].child->entries[i + MIN_ENTRIES];
    }
    node->entries[index].child->count = MIN_ENTRIES;
    for(int i = node->count; i > index; i--){
        node->entries[i] = node->entries[i - 1];
    }
    node->entries[index].child = new_node;
    node->count++;
}

void insert_into_node(NODE *node, RECT *rect){
    if(node->is_leaf){
        for(int i = 0; i < node->count; i++){
            if(node->entries[i].child == NULL){
                node->entries[i] = *rect;
                return;
            }
        }
        node->entries[node->count] = *rect;
        node->count++;
    }
    else{
        int index = choose_subtree(node, rect);
        insert_into_node(node->entries[index].child, rect);
        if(node->entries[index].child->count > MAX_ENTRIES){
            split_node(node, index);
        }
    }
}

int choose_subtree(NODE *node, RECT *rect){
    int min_area = 0;
    int min_index = 0;
    for(int i = 0; i < node->count; i++){
        int area = 1;
        for(int j = 0; j < DIMS; j++){
            area *= node->entries[i].max[j] - node->entries[i].min[j];
        }
        if(area < min_area){
            min_area = area;
            min_index = i;
        }
    }
    return min_index;
}

void print_rtree(RTREE *rtree){
    print_node(rtree->root, 0);
}

void print_node(NODE *node, int level){
    for(int i = 0; i < level; i++){
        printf(" ");
    }
    printf("Node: %d\n", level);
    for(int i = 0; i < node->count; i++){
        for(int j = 0; j < level; j++){
            printf(" ");
        }
        printf("Entry: %d\n", i);
        for(int j = 0; j < level; j++){
            printf(" ");
        }
        printf("Min: ");
        for(int j = 0; j < DIMS; j++){
            printf("%d ", node->entries[i].min[j]);
        }
        printf("\n");
        for(int j = 0; j < level; j++){
            printf(" ");
        }
        printf("Max: ");
        for(int j = 0; j < DIMS; j++){
            printf("%d ", node->entries[i].max[j]);
        }
        printf("\n");
        if(!node->is_leaf){
            print_node(node->entries[i].child, level + 1);
        }
    }
}