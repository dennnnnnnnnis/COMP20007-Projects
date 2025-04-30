/* 
leap.c

Implementations for leap list construction and manipulation.

Skeleton written by Grady Fitzaptrick for COMP20007 Assignment 1 2022
*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>
#include "leap.h"
#include "utils.h"

struct node{
    int element;
    struct node *next;
    struct node *below;
};

struct leapList {
    /* IMPLEMENT: Fill in structure. */
    int Height;
    struct node *start[INT_MAX];
    double prob;
};

void insert_uppernode(struct node *head1, struct node *head2, int key);


struct leapList *newList(int maxHeight, double p, enum problemPart part){
    /* IMPLEMENT: Set up list */
    struct leapList *newList = NULL;
    newList = malloc(sizeof(*newList));
    assert(newList != NULL);
    newList->Height = maxHeight;
    
    for (int i = 0; i < maxHeight; i++)
    {
        newList->start[i] = malloc(sizeof(*(newList->start[i]))); 
        assert(newList->start[i] != NULL);
        newList->start[i]->element = INT_MIN;
        newList->start[i]->next = NULL;
        newList->start[i]->below = NULL;
    }
    newList->start[maxHeight] = NULL;
    newList->prob = p;

    for (int j = 1; j < maxHeight; j++)
    {
        newList->start[j]->below = newList->start[j - 1];
    }

    return newList;
}

void printLevel(struct leapList *list, int level){
    if(! list){
        printf("\n");
        return;
    }
    /* IMPLEMENT (Part B): loop over list at given level, printing out each value. */
    /* Note: while additional next elements, print a space after the key. If no additional next elements, 
    print a new line and then return. */
    struct node *curr = list->start[level]->next;
    while(curr != NULL)
    {
        if (curr->next != NULL)
        {
            printf("%d ", curr->element);
            curr = curr->next;
        }
        else if (curr->next == NULL)
        {
            printf("%d", curr->element);
            break;
        }
    }
    printf("\n");
    return;
}

void insertKey(int key, struct leapList *list){
    /* IMPLEMENT: Insert the key into the given leap list. */
    int level = 0;
    struct node *newNode = malloc(sizeof(*newNode));
    assert(newNode != NULL);
    newNode->element = key;
    newNode->next = NULL;
    newNode->below = NULL;

    /* Insert at base level. */
    if (list->start[0]->next == NULL)
    {
        list->start[0]->next = newNode;
    }
    else
    {
        struct node *curr = list->start[0];
        while (curr != NULL)
        {
            if (curr->next == NULL)
            {
                curr->next = newNode;
                break;
            }
            else if (curr->next->element > key)
            {
                newNode->next = curr->next;
                curr->next = newNode;
                break;
            }
            else if (curr->next->element < key)
            {
                curr = curr->next;
            }
        }
    }
    
    for (int i = 1; i < list->Height; i++)
    {
        if ((double)rand() / RAND_MAX < list->prob)
        {
           insert_uppernode(list->start[i], list->start[level], key);
           level += 1;
        }
        else
        {
            break;
        }
    }
}

void insert_uppernode(struct node *head1, struct node *head2, int key)
{
    struct node *curr = head2;
    struct node *upperNode = malloc(sizeof(*upperNode));
    assert(upperNode != NULL);
    upperNode->element = key;
    upperNode->next = NULL;
    upperNode->below = NULL;

    while (curr != NULL)
    {
        if (curr->element != key)
        {
            curr = curr->next;
        }
        else
        {
            upperNode->below = curr;
            break;
        }
    }

    if (head1->next == NULL)
    {
        head1->next = upperNode;
    }
    else
    {
        struct node *findnode = head1;
        while (findnode != NULL)
        {
            if (findnode->next == NULL)
            {
                findnode->next = upperNode;
                break;
            }
            else if (findnode->next->element > key)
            {
                upperNode->next = findnode->next;
                findnode->next = upperNode;
                break;
            }
            else if (findnode->next->element < key)
            {
                findnode = findnode->next;
            }
        }
    }
}

/* Queries the leap list for the given key and places the result in the solution structure. */
int findKey(int key, struct leapList *list, enum problemPart part, struct solution *solution){
    int found = NOTFOUND;
    int element = key;
    int baseAccesses = 0;
    int requiredAccesses = 0;
    assert(solution);
    /* IMPLEMENT: Find the given key in the leap list. */
    int level = list->Height;
    struct node *head = list->start[list->Height - 1];

    while (head->next == NULL)
    {
        level -= 1;
        head = list->start[level - 1];
        if (level < 1)
        {
            found = NOTFOUND;
            break;
        }
    }

    struct node *curr = head;
    struct node *temp = head;
    while (curr != NULL)
    {
        if (curr->next == NULL)
        {
            curr = curr->below;
        }
        else if (curr->next->element < key)
        {
            temp = curr->next;
            curr = curr->next;
            requiredAccesses += 1;
        }
        else if (curr->next->element > key)
        {
            if (curr->next->element != temp->element)
            {
                temp = curr->next;
                requiredAccesses += 1;
            }
            curr = curr->below;
        }
        else if (curr->next->element == key)
        {
            requiredAccesses += 1;
            found = 1;
            break;
        }
    }

    struct node *base_curr = list->start[0];
    while (base_curr->next != NULL)
    {
        if (base_curr->element < key)
        {
            base_curr = base_curr->next;
            baseAccesses += 1;
        }
        else if (base_curr->element == key)
        {
            break;
        }
        else if (base_curr->next->element > key)
        {
            break;
        }
    }
    
    /* Insert result into solution. */
    (solution->queries)++;
    solution->queryResults = (int *) realloc(solution->queryResults, sizeof(int) * solution->queries);
    assert(solution->queryResults);
    (solution->queryResults)[solution->queries - 1] = found;
    solution->queryElements = (int *) realloc(solution->queryElements, sizeof(int) * solution->queries);
    assert(solution->queryElements);
    solution->queryElements[solution->queries - 1] = element;
    solution->baseAccesses = (int *) realloc(solution->baseAccesses, sizeof(int) * solution->queries);
    assert(solution->baseAccesses);
    solution->baseAccesses[solution->queries - 1] = baseAccesses;
    solution->requiredAccesses = (int *) realloc(solution->requiredAccesses, sizeof(int) * solution->queries);
    assert(solution->requiredAccesses);
    solution->requiredAccesses[solution->queries - 1] = requiredAccesses;
    return found;
}

void deleteKey(int key, struct leapList *list, enum problemPart part){
    /* IMPLEMENT: Remove the given key from the leap list. */
    struct node *head = list->start[list->Height - 1];
    int level = list->Height;

    while (head->next == NULL)
    {
        level -= 1;
        head = list->start[level - 1];
    }

    struct node *prev = NULL;
    struct node *curr = NULL;
    struct node *next_node = NULL;

    for (int i = level - 1; i >= 0; i--)
    {
        curr = list->start[i];
        while (curr->next != NULL && curr->next->element != key)
        {
            curr = curr->next;
        }
        
        if (curr->next == NULL)
        {
            continue;
        }
        else if (curr->next->element == key)
        {
            prev = curr;
            curr = curr->next;
            next_node = curr->next;
            break;
        }

    }

    while (curr->below != NULL)
    {
        curr = curr->below;
        free(prev->next);
        prev->next = next_node;
        prev = prev->below;
        next_node = curr->next;
        while (prev->next != curr)
        {
            prev = prev->next;
        }
    }

    if (curr->below == NULL)
    {
        prev->next = curr->next;
        free(curr);
    }
}

void freeList(struct leapList *list){
    /* IMPLEMENT: Free all memory used by the list. */
    for (int i = 0; i < list->Height; i++)
    {
        struct node *curr = list->start[i];
        struct node *prev = NULL;
        while (curr != NULL)
        {
            prev = curr;
            curr = curr->next;
            free(prev);
        }
    }
    free(list);
}

void freeSolution(struct solution *solution){
    if(! solution){
        return;
    }
    freeList(solution->list);
    if(solution->queries > 0){
        free(solution->queryResults);
        free(solution->queryElements);
        free(solution->baseAccesses);
        free(solution->requiredAccesses);
    }
    free(solution);
}

