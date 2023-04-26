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
    struct node *child;

} RECT;


typedef struct node{
    int count;
    RECT* entries[MAX_ENTRIES+1];
} NODE;


typedef struct rtree{
    NODE *root;
    int height;
} RTREE;


RTREE* create_rtree(){
    RTREE *rtree = (RTREE*)malloc(sizeof(RTREE));
    rtree->root = (NODE*)malloc(sizeof(NODE));
    rtree->root->count = 0;
    rtree->height=1;
    return rtree;
}

NODE* create_node(){
    NODE *node = (NODE*)malloc(sizeof(NODE));
    node->count = 0;
    for(int i = 0; i<MAX_ENTRIES+1; i++){
        node->entries[i] = (RECT*)malloc(sizeof(RECT));
        node->entries[i]->child = NULL;
        node->entries[i]->min[0] = 0;
        node->entries[i]->min[1] = 0;
        node->entries[i]->max[0] = 0;
        node->entries[i]->max[1] = 0;
    }
    return node;
}

bool is_leaf(NODE *node){
    if(node->entries[0]->child == NULL){
        return true;
    }
    return false;
}


void update_dims(NODE* node, RECT* rect, int index){
    for(int i = 0; i<DIMS; i++){
        if(node->entries[index]->min[i] > rect->min[i]){
            node->entries[index]->min[i] = rect->min[i];
        }
        if(node->entries[index]->max[i] < rect->max[i]){
            node->entries[index]->max[i] = rect->max[i];
        }
    }
}

int choose_subtree(NODE *node, RECT *rect)
{
    int min_area = 0;
    int min_add_area = 0;
    int min_index = 0;
    for (int i = 0; i < node->count; i++)
    {
        int area = 1;
        for (int j = 0; j < DIMS; j++)
        {
            area *= node->entries[i]->max[j] - node->entries[i]->min[j];
        }
        int x1 = node->entries[i]->min[0];
        int y1 = node->entries[i]->min[1];
        int x2 = node->entries[i]->max[0];
        int y2 = node->entries[i]->max[1];
        if(x1 > rect->min[0]){
            x1 = rect->min[0];
        }
        if(y1 > rect->min[1]){
            y1 = rect->min[1];
        }
        if(x2 < rect->max[0]){
            x2 = rect->max[0];
        }
        if(y2 < rect->max[1]){
            y2 = rect->max[1];
        }
        int enlarged_area = (x2 - x1) * (y2 - y1);
        int add_area = enlarged_area - area;
        if (min_add_area < add_area | i == 0)
        {
            min_area = area;
            min_index = i;
            min_add_area = add_area;
        }else if(min_add_area == add_area){
            if(min_area > area){
                min_area = area;
                min_index = i;
                min_add_area = add_area;
            }
        }
    }
    update_dims(node, rect, min_index);
    return min_index;
}

void split_node(NODE *node, int index){
    RECT* main_rect = node->entries[index];
    RECT* new_rect = (RECT*)malloc(sizeof(RECT));
    float centerx = (main_rect->max[0] + main_rect->min[0]) / 2;
    float centery = (main_rect->max[1] + main_rect->min[1]) / 2;
    NODE *split_node = main_rect->child;
    NODE *node1 = create_node();
    NODE *node2 = create_node();
    RECT* n1[MAX_ENTRIES];
    RECT* n2[MAX_ENTRIES];
    int n1_count = 0;
    int n2_count = 0;
    RECT* c1[MAX_ENTRIES];
    RECT* c2[MAX_ENTRIES];
    RECT* c3[MAX_ENTRIES];
    RECT* c4[MAX_ENTRIES];
    int c1_count = 0;
    int c2_count = 0;
    int c3_count = 0;
    int c4_count = 0;
    bool a = false;
    bool b = false;
    for(int i = 0; i< split_node->count; i++){
        float x = (split_node->entries[i]->max[0] + split_node->entries[i]->min[0]) / 2;
        float y = (split_node->entries[i]->max[1] + split_node->entries[i]->min[1]) / 2;
        if(x <= centerx && y <= centery){
            c1[c1_count] = split_node->entries[i];
            c1_count++;
        }
        else if(x >= centerx && y <= centery){
            c2[c2_count] = split_node->entries[i];
            c2_count++;
        }
        else if(x <= centerx && y >= centery){
            c4[c4_count] = split_node->entries[i];
            c4_count++;
        }
        else if(x >= centerx && y >= centery){
            c3[c3_count] = split_node->entries[i];
            c3_count++;
        }
    }
    if(c1_count > c3_count){
        a = true;
        for(int i = 0; i < c1_count; i++){
            n1[n1_count] = c1[i];
            n1_count++;
        }
        for(int i = 0; i < c3_count; i++){
            n2[n2_count] = c3[i];
            n2_count++;
        }
    }else{
        for(int i = 0; i < c3_count; i++){
            n1[n1_count] = c3[i];
            n1_count++;
        }
        for(int i = 0; i < c1_count; i++){
            n2[n2_count] = c1[i];
            n2_count++;
        }
    }
    if(c2_count > c4_count){
        b = true;
        for(int i = 0; i < c2_count; i++){
            n2[n2_count] = c2[i];
            n2_count++;
        }
        for(int i = 0; i < c4_count; i++){
            n1[n1_count] = c4[i];
            n1_count++;
        }
    }else{
        for(int i = 0; i < c4_count; i++){
            n2[n2_count] = c4[i];
            n2_count++;
        }
        for(int i = 0; i < c2_count; i++){
            n1[n1_count] = c2[i];
            n1_count++;
        }
    }
    bool splity = a^b;    
    while(n1_count < MIN_ENTRIES){
        int min_num = 0;
        for(int k = 0; k < n2_count; k++){
            if(splity){
                if(n2[k]->min[1]-centerx < n2[min_num]->min[1]-centerx){
                    min_num = k;
                }
            }else{
                if(n2[k]->min[0]-centery < n2[min_num]->min[0]-centery){
                    min_num = k;
                }
            }
        }
        n1[n1_count] = n2[min_num];
        n1_count++;
        for(int k = min_num; k < n2_count-1; k++){
            n2[k] = n2[k+1];
        }
        n2_count--;
    }
    while(n2_count < MIN_ENTRIES){
        int min_num = 0;
        for(int k = 0; k < n1_count; k++){
            if(splity){
                if(n1[k]->max[1]-centerx < n1[min_num]->max[1]-centerx){
                    min_num = k;
                }
            }else{
                if(n1[k]->max[0]-centery < n1[min_num]->max[0]-centery){
                    min_num = k;
                }
            }
        }
        n2[n2_count] = n1[min_num];
        n2_count++;
        for(int k = min_num; k < n1_count-1; k++){
            n1[k] = n1[k+1];
        }
        n1_count--;
    }
    for(int i = 0; i<n1_count; i++){
        node1->entries[i] = n1[i];
        node1->count++;
    }
    for(int i = 0; i<n2_count; i++){
        node2->entries[i] = n2[i];
        node2->count++;
    }
    main_rect->child = node1;
    new_rect->child = node2;
    node->entries[node->count] = new_rect;
    for(int i = 0; i<main_rect->child->count; i++){
        if(i==0){
            main_rect->min[0] = main_rect->child->entries[i]->min[0];
            main_rect->min[1] = main_rect->child->entries[i]->min[1];
            main_rect->max[0] = main_rect->child->entries[i]->max[0];
            main_rect->max[1] = main_rect->child->entries[i]->max[1];
        }
        else{
            if(main_rect->child->entries[i]->min[0] < main_rect->min[0]){
                main_rect->min[0] = main_rect->child->entries[i]->min[0];
            }
            if(main_rect->child->entries[i]->min[1] < main_rect->min[1]){
                main_rect->min[1] = main_rect->child->entries[i]->min[1];
            }
            if(main_rect->child->entries[i]->max[0] > main_rect->max[0]){
                main_rect->max[0] = main_rect->child->entries[i]->max[0];
            }
            if(main_rect->child->entries[i]->max[1] > main_rect->max[1]){
                main_rect->max[1] = main_rect->child->entries[i]->max[1];
            }
        }
    }
    for(int i = 0; i<new_rect->child->count; i++){
        if(i==0){
            new_rect->min[0] = new_rect->child->entries[i]->min[0];
            new_rect->min[1] = new_rect->child->entries[i]->min[1];
            new_rect->max[0] = new_rect->child->entries[i]->max[0];
            new_rect->max[1] = new_rect->child->entries[i]->max[1];
        }
        else{
            if(new_rect->child->entries[i]->min[0] < new_rect->min[0]){
                new_rect->min[0] = new_rect->child->entries[i]->min[0];
            }
            if(new_rect->child->entries[i]->min[1] < new_rect->min[1]){
                new_rect->min[1] = new_rect->child->entries[i]->min[1];
            }
            if(new_rect->child->entries[i]->max[0] > new_rect->max[0]){
                new_rect->max[0] = new_rect->child->entries[i]->max[0];
            }
            if(new_rect->child->entries[i]->max[1] > new_rect->max[1]){
                new_rect->max[1] = new_rect->child->entries[i]->max[1];
            }
        }
    }
    node->count++;
}

NODE* choose_leaf(NODE *node, RECT *rect, NODE** parents, int* indexes, int* parent_count){
    if(node->entries[0]->child == NULL){
        return node;
    }
    else{
        int index = choose_subtree(node, rect);
        parents[*parent_count] = node;
        indexes[*parent_count] = index;
        *parent_count = *parent_count + 1;
        return choose_leaf(node->entries[index]->child, rect, parents, indexes, parent_count);
    }
}

void insert_into_leaf(NODE *node, RECT *rect){
    node->entries[node->count] = rect;
    node->count++;
}

void insert_into_node(NODE *node, RECT *rect, int height){
    if(is_leaf(node)){
        insert_into_leaf(node, rect);
    }
    else{
        NODE ** parents = (NODE**)malloc(sizeof(NODE*)*height);
        int * indexes = (int*)malloc(sizeof(int)*height);
        int* count = (int*)malloc(sizeof(int));
        *count = 0;
        NODE* leaf = choose_leaf(node, rect, parents, indexes, count);
        insert_into_leaf(leaf, rect);
        for(int i = *count-1; i>=0; i--){
            if(parents[i]->entries[indexes[i]]->child->count > MAX_ENTRIES){
                split_node(parents[i], indexes[i]);
            }
        }
    }
}



void insert_into_tree(RTREE *rtree, RECT *rect){
    if(rtree->root->count == 0){
        rtree->root->entries[0] = rect;
        rtree->root->count++;
    }
    else{
        insert_into_node(rtree->root, rect, rtree->height);
        if(rtree->root->count > MAX_ENTRIES){
            NODE *new_root = create_node();

            new_root->count = 1;
            new_root->entries[0]->child = rtree->root;
            for(int i = 0; i<rtree->root->count; i++){
                if(i==0){
                    new_root->entries[0]->min[0] = rtree->root->entries[i]->min[0];
                    new_root->entries[0]->min[1] = rtree->root->entries[i]->min[1];
                    new_root->entries[0]->max[0] = rtree->root->entries[i]->max[0];
                    new_root->entries[0]->max[1] = rtree->root->entries[i]->max[1];

                }else{
                    if(rtree->root->entries[i]->min[0] < new_root->entries[0]->min[0]){
                        new_root->entries[0]->min[0] = rtree->root->entries[i]->min[0];
                    }
                    if(rtree->root->entries[i]->min[1] < new_root->entries[0]->min[1]){
                        new_root->entries[0]->min[1] = rtree->root->entries[i]->min[1];
                    }
                    if(rtree->root->entries[i]->max[0] > new_root->entries[0]->max[0]){
                        new_root->entries[0]->max[0] = rtree->root->entries[i]->max[0];
                    }
                    if(rtree->root->entries[i]->max[1] > new_root->entries[0]->max[1]){
                        new_root->entries[0]->max[1] = rtree->root->entries[i]->max[1];
                    }
                }
            }
            split_node(new_root, 0);

            rtree->root = new_root;
            rtree->height++;
        }
    }
}



void print_node(NODE *node, int level){
    for(int i = 0; i < level; i++){
        printf("    ");
    }
    printf("Node: %d\n", level);
    for(int i = 0; i < level; i++){
        printf("    ");
    }
    printf("Count: %d\n", node->count);
    for(int i = 0; i < node->count; i++){
        for(int j = 0; j < level; j++){
            printf("    ");
        }
        printf("Entry: %d\n", i); 
        if(is_leaf(node)){
            for (int j = 0; j < level; j++)
            {
                printf("    ");
            }
            printf("Leaf: X-%d Y-%d\n", node->entries[i]->min[0], node->entries[i]->min[1]);
        }else{
            for (int j = 0; j < level; j++)
            {
                printf("    ");
            }
            printf("External Node\n");
            for (int j = 0; j < level; j++)
            {
                printf("    ");
            }
            printf("Bottom Left: X-%d Y-%d\n", node->entries[i]->min[0], node->entries[i]->min[1]);
            for (int j = 0; j < level; j++)
            {
                printf("    ");
            }
            printf("Top Right: X-%d Y-%d\n", node->entries[i]->max[0], node->entries[i]->max[1]);
            print_node(node->entries[i]->child, level+1);

        }
    }
}

void print_rtree(RTREE *rtree)
{
    print_node(rtree->root, 0);
}

RECT* create_rect(int min_x, int min_y, int max_x, int max_y){
    RECT *rect = (RECT*)malloc(sizeof(RECT));
    rect->min[0] = min_x;
    rect->min[1] = min_y;
    rect->max[0] = max_x;
    rect->max[1] = max_y;
    rect->child = NULL;
    return rect;
}

int main(int c, char** v){
    FILE* fp = fopen(v[1], "r");
    int x;
    int y;
    RTREE *rtree = create_rtree();
    while(fscanf(fp, "%d %d\n", &x, &y) != EOF){
        RECT* rect = create_rect(x, y, x, y);
        insert_into_tree(rtree, rect);
    }
    print_rtree(rtree);
}