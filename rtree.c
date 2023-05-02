#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define DIMS 2
#define MAX_ENTRIES 4
#define MIN_ENTRIES 2

//each rectangle contains a <DIMS>-dimension minimum and maximum value representing two diagonaly opposite points of the rectangle
typedef struct rect{
    int min[DIMS];
    int max[DIMS];
    struct node *child;

} RECT;

//each node contains multiple rectangles
typedef struct node{
    int count;
    RECT* entries[MAX_ENTRIES+1];
} NODE;

//contains a pointer to the root of the tree and height of the tree
typedef struct rtree{
    NODE *root;
    int height;
} RTREE;

//to initialize the r tree with a node without any rectangle in it
RTREE* create_rtree(){
    RTREE *rtree = (RTREE*)malloc(sizeof(RTREE));
    rtree->root = (NODE*)malloc(sizeof(NODE));
    rtree->root->count = 0;
    rtree->height=1;
    return rtree;
}
//create a blank node with all entries having minimum and maximum values= (0,0)
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
//check if a node is leaf or not
bool is_leaf(NODE *node){
    if(node->entries[0]->child == NULL){
        return true;
    }
    return false;
}


void update_dims(NODE* node, RECT* rect, int index){ //size of rectangle changed according to the rect being added at index specified
    for(int i = 0; i<DIMS; i++){
        if(node->entries[index]->min[i] > rect->min[i]){
            node->entries[index]->min[i] = rect->min[i];
        }
        if(node->entries[index]->max[i] < rect->max[i]){
            node->entries[index]->max[i] = rect->max[i];
        }
    }
}

//to find which subtree is ideal to insert data by finding which rectangle 
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
            area *= node->entries[i]->max[j] - node->entries[i]->min[j]; // finds total area of each rectangle
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
        int enlarged_area = (x2 - x1) * (y2 - y1); // new area created if rect was inserted
        int add_area = enlarged_area - area; // additional area added
        if (min_add_area < add_area | i == 0) 
        {
            min_area = area;
            min_index = i;  // finding which node will have minimum change in area if rect was inserted
            min_add_area = add_area;
        }
        else if(min_add_area == add_area) // if the area added is equal to minimum area added calculated up till that point then add rect in the smaller rectangle
        {
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
    RECT* n1[MAX_ENTRIES+1];
    RECT* n2[MAX_ENTRIES+1];
    int n1_count = 0;
    int n2_count = 0;
    int n1_max[2] = {0,0};
    int n1_min[2] = {0,0};
    RECT* c1[MAX_ENTRIES+1];
    RECT* c2[MAX_ENTRIES+1];
    RECT* c3[MAX_ENTRIES+1];
    RECT* c4[MAX_ENTRIES+1];
    int c1_count = 0;
    int c2_count = 0;
    int c3_count = 0;
    int c4_count = 0;
    int c2_min[2] = {0,0};
    int c2_max[2] = {0,0};
    int c4_min[2] = {0, 0};
    int c4_max[2] = {0, 0};
    bool a = false;
    bool b = false;
    for(int i = 0; i< split_node->count; i++){
        float x = ((float) split_node->entries[i]->max[0] + (float) split_node->entries[i]->min[0]) / 2;
        float y = ((float) split_node->entries[i]->max[1] + (float) split_node->entries[i]->min[1]) / 2;
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
    for(int i = 0; i<c2_count; i++){
        if(c2[i]->max[0] > c2_max[0]){
            c2_max[0] = c2[i]->max[0];
        }
        if(c2[i]->max[1] > c2_max[1]){
            c2_max[1] = c2[i]->max[1];
        }
        if(c2[i]->min[0] < c2_min[0]){
            c2_min[0] = c2[i]->min[0];
        }
        if(c2[i]->min[1] < c2_min[1]){
            c2_min[1] = c2[i]->min[1];
        }
    }
    for(int i = 0; i<c4_count; i++){
        if(c4[i]->max[0] > c4_max[0]){
            c4_max[0] = c4[i]->max[0];
        }
        if(c4[i]->max[1] > c4_max[1]){
            c4_max[1] = c4[i]->max[1];
        }
        if(c4[i]->min[0] < c4_min[0]){
            c4_min[0] = c4[i]->min[0];
        }
        if(c4[i]->min[1] < c4_min[1]){
            c4_min[1] = c4[i]->min[1];
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
    for(int i = 0; i<n1_count; i++){
        if(n1[i]->max[0] > n1_max[0]){
            n1_max[0] = n1[i]->max[0];
        }
        if(n1[i]->max[1] > n1_max[1]){
            n1_max[1] = n1[i]->max[1];
        }
        if(n1[i]->min[0] < n1_min[0]){
            n1_min[0] = n1[i]->min[0];
        }
        if(n1[i]->min[1] < n1_min[1]){
            n1_min[1] = n1[i]->min[1];
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
    }else if(c4_count > c2_count){
        for(int i = 0; i < c4_count; i++){
            n2[n2_count] = c4[i];
            n2_count++;
        }
        for(int i = 0; i < c2_count; i++){
            n1[n1_count] = c2[i];
            n1_count++;
        }
    }else{
        int c2_enlarged_area = 0;
        int c4_enlarged_area = 0;
        int c2_area = (c2_max[0]-c2_min[0])*(c2_max[1]-c2_min[1]);
        int c4_area = (c4_max[0]-c4_min[0])*(c4_max[1]-c4_min[1]); 
        int n1_area = (n1_max[0]-n1_min[0])*(n1_max[1]-n1_min[1]);
        int x_min = n1_min[0];
        int x_max = n1_max[0];
        int y_min = n1_min[1];
        int y_max = n1_max[1];
        if(c2_min[0] < x_min){
            x_min = c2_min[0];
        }
        if(c2_max[0] > x_max){
            x_max = c2_max[0];
        }
        if(c2_min[1] < y_min){
            y_min = c2_min[1];
        }
        if(c2_max[1] > y_max){
            y_max = c2_max[1];
        }
        c2_enlarged_area = (x_max-x_min)*(y_max-y_min)-n1_area;
        if(c4_min[0] < x_min){
            x_min = c4_min[0];
        }
        if(c4_max[0] > x_max){
            x_max = c4_max[0];
        }
        if(c4_min[1] < y_min){
            y_min = c4_min[1];
        }
        if(c4_max[1] > y_max){
            y_max = c4_max[1];
        }
        c4_enlarged_area = (x_max-x_min)*(y_max-y_min)-n1_area;
        if(c2_enlarged_area > c4_enlarged_area){
            b = true;
            for(int i = 0; i < c2_count; i++){
                n2[n2_count] = c2[i];
                n2_count++;
            }
            for(int i = 0; i < c4_count; i++){
                n1[n1_count] = c4[i];
                n1_count++;
            }
        }else if(c4_enlarged_area > c2_enlarged_area){
            for(int i = 0; i < c4_count; i++){
                n2[n2_count] = c4[i];
                n2_count++;
            }
            for(int i = 0; i < c2_count; i++){
                n1[n1_count] = c2[i];
                n1_count++;
            }
        }else{
            if(c2_area > c4_area){
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
        n1[n1_count-1] = NULL;
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





//find ideal point of insertion of data
NODE* choose_leaf(NODE *node, RECT *rect, NODE** parents, int* indexes, int* parent_count){
    if(is_leaf(node)){ // leaf node found for insertion
        return node;
    }
    else{
        int index = choose_subtree(node, rect); //current node is not node so searching deeper
        parents[*parent_count] = node; //current node entered into the parents array
        indexes[*parent_count] = index;  // indexes in each node where rect was inserted
        *parent_count = *parent_count + 1;
        return choose_leaf(node->entries[index]->child, rect, parents, indexes, parent_count); //recursively trying to find the leaf 
    }
}

void insert_into_leaf(NODE *node, RECT *rect){ //inserts rect into a leaf node 
    node->entries[node->count] = rect;
    node->count++;
}

void adjust_tree(NODE** parents, int* indexes, int count){
    for (int i = count - 1; i >= 0; i--) // traverses the parents array backwards so that if a previous splitting causes some parent node to have more than 4 entries then it can be split again using the parent's parent
    {
        if (parents[i]->entries[indexes[i]]->child->count > MAX_ENTRIES) // if number of entries in the current node where rect was inserted is greater than MAX_ENTRIES then do node splitting
        {
            split_node(parents[i], indexes[i]);
        }
    }
}



void insert_into_node(NODE *node, RECT *rect, int height){
    if(is_leaf(node)){  // if node is a leaf node insert into leaf as one of the entries
        insert_into_leaf(node, rect);
    }
    else{
        NODE ** parents = (NODE**)malloc(sizeof(NODE*)*height);//an array of parent nodes containing nodes traversed till current position reached by traversal
        int* indexes = (int*)malloc(sizeof(int)*height); //array containing which specific index was chosen in each node
        int* count = (int*)malloc(sizeof(int));
        *count = 0;
        NODE* leaf = choose_leaf(node, rect, parents, indexes, count); // parents contains all the nodes traversed while finding the leaf node, indexes contains the entry index in each node whose child was taken as next node
        insert_into_leaf(leaf, rect); // insert into the position found
        adjust_tree(parents, indexes, *count); // adjust the tree if any node has more than 4 entries
        
    }
}



void insert_into_tree(RTREE *rtree, RECT *rect){
    if(rtree->root->count == 0) // if no entires in root node only increase count and add rect as first entry
    {
        rtree->root->entries[0] = rect;
        rtree->root->count++;
    }
    else{
        insert_into_node(rtree->root, rect, rtree->height);
        if(rtree->root->count > MAX_ENTRIES) // special case of node spitting for root node
        {
            NODE *new_root = create_node();

            new_root->count = 1;
            new_root->entries[0]->child = rtree->root; // new_root will have one entry enclosing all the rectangles in root
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

            rtree->root = new_root; // newly created node set as root
            rtree->height++;
        }
    }
}

void print_node(NODE* node, int level, int* total)
{
    for (int i = 0; i < level; i++)
    {
        printf("    ");
    }
    printf("External Node: %d\n", level);
    for (int i = 0; i < level; i++)
    {
        printf("    ");
    }
    printf("Count: %d\n", node->count); // prints the count
    for (int i = 0; i < node->count; i++)
    {
        for (int j = 0; j < level; j++)
        {
            printf("    ");
        }
        printf("Rectangle Number: %d\n", i);
        if (is_leaf(node))
        { // if the node is leaf print the leaf
            for (int j = 0; j < level; j++)
            {
                printf("    ");
            }
            printf("Leaf: X-%d Y-%d\n", node->entries[i]->min[0], node->entries[i]->min[1]);
            *total = *total + 1;
        }
        else
        {
            for (int j = 0; j < level; j++)
            {
                printf("    ");
            }
            printf("MBR\n");
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
            print_node(node->entries[i]->child, level + 1, total); // if it is not leaf go deeper and print its child
        }
    }
}

void print_rtree(RTREE *rtree, int* total)
{
    print_node(rtree->root, 0, total);
}

RECT* create_rect(int min_x, int min_y, int max_x, int max_y){ // creates an instance of struct rect
    RECT *rect = (RECT*)malloc(sizeof(RECT));
    rect->min[0] = min_x;
    rect->min[1] = min_y;
    rect->max[0] = max_x;
    rect->max[1] = max_y;
    rect->child = NULL;
    return rect;
}

int main(int argc, char const *argv[]){
    FILE* fp = fopen(argv[1], "r");
    int x;
    int y;
    RTREE *rtree = create_rtree();
    while(fscanf(fp, "%d %d\n", &x, &y) != EOF){
        RECT* rect = create_rect(x, y, x, y); // reading 2d data line by line and making it a rectangle and inserting into tree
        insert_into_tree(rtree, rect);
    }
    int* total = (int*)malloc(sizeof(int));
    *total = 0;
    print_rtree(rtree, total);
    printf("Total Leaf Nodes: %d\n", *total);
}