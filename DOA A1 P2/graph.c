/*
graph.c

Set of vertices and edges implementation.

Implementations for helper functions for graph construction and manipulation.

Skeleton written by Grady Fitzpatrick for COMP20007 Assignment 1 2022
*/
#include <stdlib.h>
#include <assert.h>
#include <limits.h>
#include "graph.h"
#include "utils.h"
#include "pq.h"

#define INITIALEDGES 32

struct edge;

/* Definition of a graph. */
struct graph {
  int numVertices;
  int numEdges;
  int allocedEdges;
  struct edge **edgeList;
};

/* Definition of an edge. */
struct edge {
  int start;
  int end;
  int cost;
};

struct pq {
  int count; // how many number in queue
  int allocated; // queue有多大空间
  void **queue; // 空间里面带着数字
  int *priorities; // 上一层
};

int shortest_distance(struct graph *g, int numRooms, int startingRoom, int bossRoom);

struct graph *newGraph(int numVertices){
  struct graph *g = (struct graph *) malloc(sizeof(struct graph));
  assert(g);
  /* Initialise edges. */
  g->numVertices = numVertices;
  g->numEdges = 0;
  g->allocedEdges = 0;
  g->edgeList = NULL;
  return g;
}

/* Adds an edge to the given graph. */
void addEdge(struct graph *g, int start, int end, int cost){
  assert(g);
  struct edge *newEdge = NULL;
  /* Check we have enough space for the new edge. */
  if((g->numEdges + 1) > g->allocedEdges){
    if(g->allocedEdges == 0){
      g->allocedEdges = INITIALEDGES;
    } else {
      (g->allocedEdges) *= 2;
    }
    g->edgeList = (struct edge **) realloc(g->edgeList,
      sizeof(struct edge *) * g->allocedEdges);
    assert(g->edgeList);
  }

  /* Create the edge */
  newEdge = (struct edge *) malloc(sizeof(struct edge));
  assert(newEdge);
  newEdge->start = start;
  newEdge->end = end;
  newEdge->cost = cost;

  /* Add the edge to the list of edges. */
  g->edgeList[g->numEdges] = newEdge;
  (g->numEdges)++;
}

/* Returns a new graph which is a deep copy of the given graph (which must be 
  freed with freeGraph when no longer used). */
struct graph *duplicateGraph(struct graph *g){
  struct graph *copyGraph = (struct graph *) malloc(sizeof(struct graph));
  assert(copyGraph);
  copyGraph->numVertices = g->numVertices;
  copyGraph->numEdges = g->numEdges;
  copyGraph->allocedEdges = g->allocedEdges;
  copyGraph->edgeList = (struct edge **) malloc(sizeof(struct edge *) * g->allocedEdges);
  assert(copyGraph->edgeList || copyGraph->numEdges == 0);
  int i;
  /* Copy edge list. */
  for(i = 0; i < g->numEdges; i++){
    struct edge *newEdge = (struct edge *) malloc(sizeof(struct edge));
    assert(newEdge);
    newEdge->start = (g->edgeList)[i]->start;
    newEdge->end = (g->edgeList)[i]->end;
    newEdge->cost = (g->edgeList)[i]->cost;
    (copyGraph->edgeList)[i] = newEdge;
  }
  return copyGraph;
}

/* Frees all memory used by graph. */
void freeGraph(struct graph *g){
  int i;
  for(i = 0; i < g->numEdges; i++){
    free((g->edgeList)[i]);
  }
  if(g->edgeList){
    free(g->edgeList);
  }
  free(g);
}

int shortest_distance(struct graph *g, int numRooms, int startingRoom, int bossRoom)
{
  int adjmatrix[numRooms][numRooms];
  int priority[numRooms];
  int candidate_priority[numRooms];
  int curr = startingRoom;
  int empty = 0; // priority is not empty
  int find = 0;
  int count = 0;
  int candidate_empty = 0;

  for (int i = 0; i < numRooms; i++)
  {
    priority[i] = 0;
    candidate_priority[i] = 0;
    for (int j = 0; j < numRooms; j++)
    {
      adjmatrix[i][j] = 0;
    }
  }

  // first priority is 0
  for (int i = 0; i < g->numEdges; i++)
  {
    if (g->edgeList[i]->start == curr && adjmatrix[curr][g->edgeList[i]->end] == 0)
    {
      int pass = 1;
      for (int k = 0; k < numRooms; k++)
      {
        if (adjmatrix[k][g->edgeList[i]->end] == 1)
        {
          pass = 0;
          break;
        }
      }
      if (pass == 1)
      {
        adjmatrix[curr][g->edgeList[i]->end] = 1;
        adjmatrix[g->edgeList[i]->end][curr] = 1;
        candidate_priority[g->edgeList[i]->end] = g->edgeList[i]->end;
      }
    }
    else if (g->edgeList[i]->end == curr && adjmatrix[g->edgeList[i]->start][curr] == 0)
    {
      int pass = 1;
      for (int k = 0; k < numRooms; k++)
      {
        if (adjmatrix[k][g->edgeList[i]->start] == 1)
        {
          pass = 0;
          break;
        }
      }
      if (pass == 1)
      {
        adjmatrix[g->edgeList[i]->start][curr] = 1;
        adjmatrix[curr][g->edgeList[i]->start] = 1;
        candidate_priority[g->edgeList[i]->start] = g->edgeList[i]->start;
      }
    }
  }
    
 while (empty != 1)
  {
    for (int j = 1; j < numRooms; j++)
    {
      if (priority[j] != 0)
      {
        empty = 0;
        curr = priority[j];
        priority[j] = 0;
        for (int i = 0; i < g->numEdges; i++)
        {
          if (g->edgeList[i]->start == curr && adjmatrix[curr][g->edgeList[i]->end] == 0)
          {
            int pass = 1;
            for (int k = 0; k < numRooms; k++)
            {
              if (adjmatrix[k][g->edgeList[i]->end] == 1)
              {
                pass = 0;
                break;
              }
            }
            if (pass == 1)
            {
              adjmatrix[curr][g->edgeList[i]->end] = 1;
              adjmatrix[g->edgeList[i]->end][curr] = 1;
              candidate_priority[g->edgeList[i]->end] = g->edgeList[i]->end;
              candidate_empty = 0;
            }
          }
          else if (g->edgeList[i]->end == curr && adjmatrix[g->edgeList[i]->start][curr] == 0)
          {
            int pass = 1;
            for (int k = 0; k < numRooms; k++)
            {
              if (adjmatrix[k][g->edgeList[i]->start] == 1)
              {
                pass = 0;
                break;
              }
            }
            if (pass == 1)
            {
              adjmatrix[g->edgeList[i]->start][curr] = 1;
              adjmatrix[curr][g->edgeList[i]->start] = 1;
              candidate_priority[g->edgeList[i]->start] = g->edgeList[i]->start;
              candidate_empty = 0;
            }
          }
        }
      } 
      for (int iter = 0; iter < numRooms; iter++)
      {
        if (candidate_priority[iter] == bossRoom)
        {
          find = 1;
          count += 1;
          break; // no need to iterate since already find the bossroom
        }
      }
      if (find == 1)
      {
        break; // no need to find the next candidate since bossroom has been found
      }
    }
    empty = 1;
    
    if (find == 1)
    {
      break;  
    }
    if (empty == 1 && candidate_empty == 0)
    {
      for (int i = 0; i < numRooms; i++)
      {
        priority[i] = candidate_priority[i];
        candidate_priority[i] = 0;
        empty = 0;
      }
      candidate_empty = 1;
    }
    count += 1;
  }

  int find_boss = 0;
  for (int i = 0; i < numRooms; i++)
  {
    if (adjmatrix[i][bossRoom] == 1)
    {
      find_boss = 1;
      break;
    }
  }

  if (find_boss == 1)
  {
    return count;
  }
  else
  {
    return INT_MAX;
  }
    
}

struct solution *graphSolve(struct graph *g, enum problemPart part,
  int numRooms, int startingRoom, int bossRoom, int numShortcuts, 
  int *shortcutStarts, int *shortcutEnds, int numHeartRooms, int *heartRooms){
  struct solution *solution = (struct solution *)
    malloc(sizeof(struct solution));
  assert(solution);
  if(part == PART_A){
    /* IMPLEMENT 2A SOLUTION HERE */
    solution->heartsLost = 0;
    solution->heartsLost += shortest_distance(g, numRooms, startingRoom, bossRoom);
  } else if(part == PART_B) {
    /* IMPLEMENT 2B SOLUTION HERE */
    solution->heartsLost = 0;
    int shortcuts[numShortcuts];
    int shortest = shortest_distance(g, numRooms, startingRoom, bossRoom);
    for (int j = 0; j < numShortcuts; j++)
    {
      shortcuts[j] = 0;
    }

    for (int i = 0; i < numShortcuts; i++)
    {
      struct graph *copied_graph = NULL;
      copied_graph = duplicateGraph(g);
      addEdge(copied_graph, shortcutStarts[i], shortcutEnds[i], 1);
      shortcuts[i] = shortest_distance(copied_graph, numRooms, startingRoom, bossRoom);
      freeGraph(copied_graph);
    }
    for (int i = 0; i < numShortcuts; i++)
    {
      if (shortcuts[i] < shortest)
      {
        shortest = shortcuts[i];
      }
    }

    solution->heartsLost += shortest;
  } else {
    /* IMPLEMENT 2C SOLUTION HERE */
    solution->heartsLost = 0;
    int adjmatrix[numRooms][numRooms];
    int tree[numRooms][numRooms];
    int shortest_route[numRooms];
    int priority[numRooms];
    int candidate_priority[numRooms];
    int curr = startingRoom;
    int empty = 0; // priority is not empty
    int count = 0;
    int index = 1;
    int candidate_empty = 0;

    for (int i = 0; i < numRooms; i++)
    {
      priority[i] = 0;
      candidate_priority[i] = 0;
      for (int j = 0; j < numRooms; j++)
      {
        adjmatrix[i][j] = 0;
        tree[i][j] = 0;
      }
    }

    // first priority is 0
    for (int i = 0; i < g->numEdges; i++)
    {
      if (g->edgeList[i]->start == curr && adjmatrix[curr][g->edgeList[i]->end] == 0)
      {
        int pass = 1;
        for (int k = 0; k < numRooms; k++)
        {
          if (adjmatrix[k][g->edgeList[i]->end] == 1)
          {
            pass = 0;
            break;
          }
        }
        if (pass == 1)
        {
          adjmatrix[curr][g->edgeList[i]->end] = 1;
          adjmatrix[g->edgeList[i]->end][curr] = 1;
          candidate_priority[g->edgeList[i]->end] = g->edgeList[i]->end;
          tree[curr][g->edgeList[i]->end] = g->edgeList[i]->end;
        }
      }
      else if (g->edgeList[i]->end == curr && adjmatrix[g->edgeList[i]->start][curr] == 0)
      {
        int pass = 1;
        for (int k = 0; k < numRooms; k++)
        {
          if (adjmatrix[k][g->edgeList[i]->start] == 1)
          {
            pass = 0;
            break;
          }
        }
        if (pass == 1)
        {
          adjmatrix[g->edgeList[i]->start][curr] = 1;
          adjmatrix[curr][g->edgeList[i]->start] = 1;
          candidate_priority[g->edgeList[i]->start] = g->edgeList[i]->start;
          tree[curr][g->edgeList[i]->start] = g->edgeList[i]->start;
        }
      }
    }
      
  while (empty != 1)
    {
      while (index < numRooms)
      {
        for (int iter = 0; iter < numHeartRooms; iter++)
        {
          if (priority[heartRooms[iter]] != 0)
          {
            empty = 0;
            curr = priority[heartRooms[iter]];
            priority[heartRooms[iter]] = 0;
            break;
          }
        }
        if (curr == 0)
        {
          if (priority[index] != 0)
          {
            empty = 0;
            curr = priority[index];
            priority[index] = 0;
          }
          index++;
        }
        for (int i = 0; i < g->numEdges; i++)
        {
          if (g->edgeList[i]->start == curr && adjmatrix[curr][g->edgeList[i]->end] == 0)
          {
            int pass = 1;
            for (int k = 0; k < numRooms; k++)
            {
              if (adjmatrix[k][g->edgeList[i]->end] == 1)
              {
                if (g->edgeList[i]->end != bossRoom)
                {
                  pass = 0;
                  break;
                }
                else
                {
                  pass = 1;
                  break;
                }
              }
            }
            if (pass == 1)
            {
              adjmatrix[curr][g->edgeList[i]->end] = 1;
              adjmatrix[g->edgeList[i]->end][curr] = 1;
              if (g->edgeList[i]->end != bossRoom)
              {
                candidate_priority[g->edgeList[i]->end] = g->edgeList[i]->end;
              }
              tree[curr][g->edgeList[i]->end] = g->edgeList[i]->end;
              candidate_empty = 0;
            }
          }
          else if (g->edgeList[i]->end == curr && adjmatrix[g->edgeList[i]->start][curr] == 0)
          {
            int pass = 1;
            for (int k = 0; k < numRooms; k++)
            {
              if (adjmatrix[k][g->edgeList[i]->start] == 1)
              {
                if (g->edgeList[i]->start != bossRoom)
                {
                  pass = 0;
                  break;
                }
                else
                {
                  pass = 1;
                  break;
                }
              }
            }
            if (pass == 1)
            {
              adjmatrix[g->edgeList[i]->start][curr] = 1;
              adjmatrix[curr][g->edgeList[i]->start] = 1;
              if (g->edgeList[i]->start != bossRoom)
              {
                candidate_priority[g->edgeList[i]->start] = g->edgeList[i]->start;
              }
              tree[curr][g->edgeList[i]->start] = g->edgeList[i]->start;
              candidate_empty = 0;
            }
          }
        } 
        curr = 0; 
      }
      empty = 1;

      if (empty == 1 && candidate_empty == 0)
      {
        for (int i = 0; i < numRooms; i++)
        {
          priority[i] = candidate_priority[i];
          candidate_priority[i] = 0;
          empty = 0;
        }
        candidate_empty = 1;
      }
      index = 1;
    }

    int fastest = INT_MAX;

    for (int k = 0; k < numRooms; k++)
    {
      for (int i = 0; i < numRooms; i++)
      {
        shortest_route[i] = 0; 
      }
      int stop = bossRoom;
      shortest_route[bossRoom] = bossRoom;
      if (tree[k][bossRoom] == bossRoom)
      {
        shortest_route[k] = k;
        stop = k;
        while (stop != startingRoom)
        {
          for (int iter = 0; iter < numRooms; iter++)
          {
            if (tree[iter][stop] == stop)
            {
              shortest_route[iter] = iter;
              stop = iter;
            }

            if (stop == startingRoom)
            {
              break;
            }
          }
        }

        int counted = 1; // should count
        for (int a = 0; a < numRooms; a++)
        {
          for (int j = 0; j < numHeartRooms; j++)
          {
            if (shortest_route[a] != heartRooms[j] && shortest_route[a] != 0)
            {
              counted = 1;
            }
            else
            {
              counted = 0;
              break;
            }
          }
          if (counted == 1)
          {
            count += 1;
          }
        }
        if (count < fastest)
        {
          fastest = count;
        }

      }
      count = 0;
    }

  solution->heartsLost += fastest;

  }
  return solution;
}

