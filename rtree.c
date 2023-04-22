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
    RECT entries[MAX_ENTRIES+1];
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

void split_node(NODE *node, int index){
    RECT main_rect = node->entries[index];
    RECT* new_rect = (RECT*)malloc(sizeof(RECT));
    float centerx = (main_rect.max[0] + main_rect.min[0]) / 2;
    float centery = (main_rect.max[1] + main_rect.min[1]) / 2;
    NODE *split_node = main_rect.child;
    RECT n1[MAX_ENTRIES];
    RECT n2[MAX_ENTRIES];
    int n1_count = 0;
    int n2_count = 0;
    RECT c1[MAX_ENTRIES];
    RECT c2[MAX_ENTRIES];
    RECT c3[MAX_ENTRIES];
    RECT c4[MAX_ENTRIES];
    int c1_count = 0;
    int c2_count = 0;
    int c3_count = 0;
    int c4_count = 0;
    for(int i = 0; i< split_node->count; i++){
        float x = (split_node->entries[i].max[0] + split_node->entries[i].min[0]) / 2;
        float y = (split_node->entries[i].max[1] + split_node->entries[i].min[1]) / 2;
        if(x < centerx && y < centery){
            c1[c1_count] = split_node->entries[i];
            c1_count++;
        }
        else if(x > centerx && y < centery){
            c2[c2_count] = split_node->entries[i];
            c2_count++;
        }
        else if(x < centerx && y > centery){
            c3[c3_count] = split_node->entries[i];
            c3_count++;
        }
        else if(x > centerx && y > centery){
            c4[c4_count] = split_node->entries[i];
            c4_count++;
        }
    }
    if(c1_count > c4_count){
        for(int i = 0; i < c1_count; i++){
            n1[n1_count] = c1[i];
            n1_count++;
        }
        for(int i = 0; i < c4_count; i++){
            n2[n2_count] = c4[i];
            n2_count++;
        }
    }else{
        for(int i = 0; i < c4_count; i++){
            n1[n1_count] = c4[i];
            n1_count++;
        }
        for(int i = 0; i < c1_count; i++){
            n2[n2_count] = c1[i];
            n2_count++;
        }
    }
    if(c2_count > c3_count){
        for(int i = 0; i < c2_count; i++){
            n2[n2_count] = c2[i];
            n2_count++;
        }
        for(int i = 0; i < c3_count; i++){
            n1[n1_count] = c3[i];
            n1_count++;
        }
    }else{
        for(int i = 0; i < c3_count; i++){
            n2[n2_count] = c3[i];
            n2_count++;
        }
        for(int i = 0; i < c2_count; i++){
            n1[n1_count] = c2[i];
            n1_count++;
        }
    }
    main_rect.child = n1;
    new_rect->child = n2;
    node->entries[index] = main_rect;
    node->entries[node->count] = *new_rect;
    for(int i = 0; i<main_rect.child->count; i++){
        if(i==0){
            main_rect.min[0] = main_rect.child->entries[i].min[0];
            main_rect.min[1] = main_rect.child->entries[i].min[1];
            main_rect.max[0] = main_rect.child->entries[i].max[0];
            main_rect.max[1] = main_rect.child->entries[i].max[1];
        }
        else{
            if(main_rect.child->entries[i].min[0] < main_rect.min[0]){
                main_rect.min[0] = main_rect.child->entries[i].min[0];
            }
            if(main_rect.child->entries[i].min[1] < main_rect.min[1]){
                main_rect.min[1] = main_rect.child->entries[i].min[1];
            }
            if(main_rect.child->entries[i].max[0] > main_rect.max[0]){
                main_rect.max[0] = main_rect.child->entries[i].max[0];
            }
            if(main_rect.child->entries[i].max[1] > main_rect.max[1]){
                main_rect.max[1] = main_rect.child->entries[i].max[1];
            }
        }
    }
    for(int i = 0; i<new_rect->child->count; i++){
        if(i==0){
            new_rect->min[0] = new_rect->child->entries[i].min[0];
            new_rect->min[1] = new_rect->child->entries[i].min[1];
            new_rect->max[0] = new_rect->child->entries[i].max[0];
            new_rect->max[1] = new_rect->child->entries[i].max[1];
        }
        else{
            if(new_rect->child->entries[i].min[0] < new_rect->min[0]){
                new_rect->min[0] = new_rect->child->entries[i].min[0];
            }
            if(new_rect->child->entries[i].min[1] < new_rect->min[1]){
                new_rect->min[1] = new_rect->child->entries[i].min[1];
            }
            if(new_rect->child->entries[i].max[0] > new_rect->max[0]){
                new_rect->max[0] = new_rect->child->entries[i].max[0];
            }
            if(new_rect->child->entries[i].max[1] > new_rect->max[1]){
                new_rect->max[1] = new_rect->child->entries[i].max[1];
            }
        }
    }
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
        if(area < min_area | i == 0){
            min_area = area;
            min_index = i;
        }

    }
    return min_index;
}

void insert_into_tree(RTREE *rtree, RECT *rect){
    if(rtree->root->count == 0){
        rtree->root->entries[0] = *rect;
        rtree->root->count++;
    }
    else{
        insert_into_node(rtree->root, rect);
        if(rtree->root->count > MAX_ENTRIES){
            NODE *new_root = create_node();
            new_root->is_leaf = 0;
            new_root->count = 1;
            new_root->entries[0].child = rtree->root;
            split_node(new_root, 0);
            rtree->root = new_root;
        }
    }
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