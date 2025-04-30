/* 
234tree.c

Implementation for a 2-3-4 tree.

Original solution written by Grady Fitzpatrick for COMP20007 Week 9 Workshop
    with modifications for skeleton.
*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "234tree.h"

/* These constants are not responsive, changing them from their value will
likely cause issues in the operation of the tree without other changes. */
#define MAXCHILDREN 4
#define MAXVALUES 3

struct tree {
    struct tree *child0;
    struct tree *child1;
    struct tree *child2;
    struct tree *child3;
    int value1;
    int value2;
    int value3;
    int numVals;
    int numDecendant;
};

/* Inserts a value into the given node. */
void insertIntoNode(struct tree *tree, int value);
/* Split the middle value from the child into the tree node and insert into 
the tree node recursively. */
struct tree *splitAndInsert(struct tree *tree, struct tree *child, int value);
/* Insert the item into the tree using pre-emptive splitting. */
struct tree *insertTreeRecursive(struct tree *tree, int value);

/* Find the total number of the values it has of this tree given its root. */
int totalnum(struct tree *tree);
/* Find the total number of decendants a given root has. */
int numDecendants(struct tree *tree);
/* Find the median value using in-order traversal */
void findmid(struct tree *tree, int level, int *median_index, int *value, int *target_value);

void insertIntoNode(struct tree *tree, int value){
    if(tree->numVals == 1){
        tree->value2 = value;
    } else if(tree->numVals == 2){
        tree->value3 = value;
    }
    tree->numVals = tree->numVals + 1;

    /* insertionSort(tree->value[1..3]), all values before final are sorted. */
    if(tree->numVals > 2){
      if(tree->value3 < tree->value2){
        tree->value3 = tree->value2;
        tree->value2 = value;
      }
    }
    if(tree->numVals > 1){
      if(tree->value2 < tree->value1){
        tree->value2 = tree->value1;
        tree->value1 = value;
      }
    }
}

struct tree *splitAndInsert(struct tree *tree, struct tree *child, int value){
    /* Take median value, place in the tree. */

    /* Find which child the value being split up comes from. */
    int insertLocation = 0;
    if(tree->child0 == child){
        insertLocation = 0;
    } else if(tree->child1 == child){
        insertLocation = 1;
    } else if(tree->child2 == child){
        insertLocation = 2;
    } else if(tree->child3 == child){
        insertLocation = 3;
    }

    /* Insert into empty trees to lazily do allocation. */
    struct tree *left = insertTree(NULL, child->value1);
    struct tree *right = insertTree(NULL, child->value3);
    left->child0 = child->child0;
    left->child1 = child->child1;
    right->child0 = child->child2;
    right->child1 = child->child3;

    switch(insertLocation){
        case 0:
            /* Inserting in first value, shuffle all along. */
            tree->child3 = tree->child2;
            tree->value3 = tree->value2;
            tree->child2 = tree->child1;
            tree->value2 = tree->value1;
            /* Place median child value in 1st value. */
            tree->value1 = child->value2;
            tree->child0 = left;
            tree->child1 = right;
            break;
        case 1:
            /* Inserting in second value, shuffle all after first along. */
            tree->child3 = tree->child2;
            tree->value3 = tree->value2;
            /* Place median child value in 2nd value. */
            tree->value2 = child->value2;
            tree->child1 = left;
            tree->child2 = right;
            break;
        case 2:
            /* Inserting in third value, no shuffling needed. */
            /* Place median child value in 3rd value. */
            tree->value3 = child->value2;
            tree->child2 = left;
            tree->child3 = right;
            break;
        case 3:
            /* Child being pushed up being the third child implies the node is
              a 4-node. This should never happen as the tree node should have
              been split before its child was checked (pre-emptive split).
            */
            assert(insertLocation < MAXVALUES);
            break;
        default:
            /* Should be 2-3-4. */
            assert(insertLocation < MAXCHILDREN && insertLocation >= 0);
    }
    /* Value inserted now, so increase number of items by one. */
    tree->numVals = tree->numVals + 1;

    /* Clean up replaced child node. */
    free(child);
    return insertTreeRecursive(tree, value);
}

/* Handles the first node, where either:
  1. The tree contains 3-elements at the root and must be pre-emptively split.
  2. The tree contains no nodes and the root must be created.
  3. The tree does not contain 3-elements and the usual insertion begins.
*/
struct tree *insertTree(struct tree *tree, int value){
    if(tree == NULL){
        tree = (struct tree *) malloc(sizeof(struct tree));
        assert(tree);
        tree->child0 = NULL;
        tree->child1 = NULL;
        tree->child2 = NULL;
        tree->child3 = NULL;
        tree->value1 = value;
        tree->value2 = -1;
        tree->value3 = -1;
        tree->numVals = 1;
        tree->numDecendant = -1;
    } else {
        if(tree->numVals == 3){
            /* Insert into empty trees to lazily do allocation. */
            struct tree *left = insertTree(NULL, tree->value1);
            struct tree *root = insertTree(NULL, tree->value2);
            struct tree *right = insertTree(NULL, tree->value3);
            /* Distribute children. */
            left->child0 = tree->child0;
            left->child1 = tree->child1;
            right->child0 = tree->child2;
            right->child1 = tree->child3;
            root->child0 = left;
            root->child1 = right;
            /* Clean up old node */
            free(tree);
            tree = root;
        }
        tree = insertTreeRecursive(tree, value);
    }

    return tree;
}

struct tree *insertTreeRecursive(struct tree *tree, int value){

    if(tree->child0 == NULL){
        insertIntoNode(tree, value);
    } else if(value < tree->value1) {
        if(tree->child0->numVals > (MAXVALUES - 1)){
            tree = splitAndInsert(tree, tree->child0, value);
        } else {
            tree->child0 = insertTreeRecursive(tree->child0, value);
        }
    } else if(tree->numVals < 2 || value < tree->value2){
        if(tree->child1->numVals > (MAXVALUES - 1)){
            tree = splitAndInsert(tree, tree->child1, value);
        } else {
            tree->child1 = insertTreeRecursive(tree->child1, value);
        }
    } else if(tree->numVals < 3 || value < tree->value3){
        if(tree->child2->numVals > (MAXVALUES - 1)){
            tree = splitAndInsert(tree, tree->child2, value);
        } else {
            tree->child2 = insertTreeRecursive(tree->child2, value);
        }
    } else {
        if(tree->child3->numVals > (MAXVALUES - 1)){
            tree = splitAndInsert(tree, tree->child3, value);
        } else {
            tree->child3 = insertTreeRecursive(tree->child3, value);
        }
    }
    return tree;
}

void printTree(struct tree *tree, int level){
    if(tree == NULL){
        return;
    }
    /* Pre-order traversal. */
    printf("[ %d, %d, %d ] (level %d)\n", tree->value1, tree->value2, tree->value3, level);

    printTree(tree->child0, level + 1);
    //if(tree->value1 != (-1)){
    //    printf("%d (level %d)\n", tree->value1, level);
    //}
    printTree(tree->child1, level + 1);
    //if(tree->value2 != (-1)){
    //    printf("%d (level %d)\n", tree->value2, level);
    //}
    printTree(tree->child2, level + 1);
    //if(tree->value3 != (-1)){
    //    printf("%d (level %d)\n", tree->value3, level);
    //}
    printTree(tree->child3, level + 1);
}

void findMedian(struct tree *tree){
    int level = 0;
    int value = 0;
    int target_level = 0;
    
    numDecendants(tree);
    int num_input = totalnum(tree);
    int median_index = (num_input + 1) / 2;
    
    findmid(tree, level, &median_index, &value, &target_level);

    printf("%d %d\n", value, target_level);
}

void findmid(struct tree *tree, int level, int *median_index, int *value, int *target_level)
{
    if (tree != NULL)
    {
        findmid(tree->child0, level + 1, median_index, value, target_level);
        // printf("%d", tree->value1);
        if (tree->value1 != -1)
        {
            *median_index -= 1;
            if (*median_index == 0)
            {
                *value = tree->value1;
                *target_level = level;
            }
        }

        if (*median_index != 0)
        {
            findmid(tree->child1, level + 1, median_index, value, target_level);
            // printf("%d", tree->value2);
            if (tree->value2 != -1)
            {
                *median_index -= 1;
                if (*median_index == 0)
                {
                    *value = tree->value2;
                    *target_level = level;
                }
            }
        }

        if (*median_index != 0)
        {
            findmid(tree->child2, level + 1, median_index, value, target_level);
            // printf("%d", tree->value3);
            if (tree->value3 != -1)
            {
                *median_index -= 1;
                if (*median_index == 0)
                {
                    *value = tree->value3;
                    *target_level = level;
                }
            }
        }

        if (*median_index != 0)
        {
            findmid(tree->child3, level + 1, median_index, value, target_level);
        }
    }
}

int totalnum(struct tree *tree)
{
    if (tree == NULL)
    {
        return 0;
    }
    else
    {
        return totalnum(tree->child0) + totalnum(tree->child1)
         + totalnum(tree->child2) + totalnum(tree->child3) + tree->numVals;
    }
}

int numDecendants(struct tree *tree)
{
    if (tree == NULL)
    {
        return 0;
    }
    else
    {
        int decendants = totalnum(tree->child0) + totalnum(tree->child1)
         + totalnum(tree->child2) + totalnum(tree->child3);
         tree->numDecendant = decendants;
        return decendants;
    }
}

