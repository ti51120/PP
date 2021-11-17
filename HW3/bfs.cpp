#include "bfs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cstddef>
#include <omp.h>

#include "../common/CycleTimer.h"
#include "../common/graph.h"

#define ROOT_NODE_ID 0
#define NOT_VISITED_MARKER -1
#define beta 24
// #define VERBOSE

void vertex_set_clear(vertex_set *list)
{
    list->count = 0;
}

void vertex_set_init(vertex_set *list, int count)
{
    list->max_vertices = count;
    list->vertices = (int *)malloc(sizeof(int) * list->max_vertices);
    vertex_set_clear(list);
}

void top_down_step(Graph g, vertex_set *frontier, int *distances, int* depth){
    int local_count = 0;
    #pragma omp parallel for reduction(+:local_count)
    for(int node = 0; node < g->num_nodes; ++node){
        if(frontier->vertices[node] == *depth){

            const Vertex* v_start = outgoing_begin(g, node);
            const Vertex* v_end = outgoing_end(g, node);
            
            for (const Vertex* v=v_start; v!=v_end; v++){
                if(frontier->vertices[*v] == NOT_VISITED_MARKER){
                    local_count++;
                    distances[*v] = distances[node] + 1;
                    frontier->vertices[*v] = *depth + 1;
                }
            }
        }
    }
    frontier->count = local_count;
}
// Implements top-down BFS.
//
// Result of execution is that, for each node in the graph, the
// distance to the root is stored in sol.distances.
void bfs_top_down(Graph graph, solution *sol){

    vertex_set list1;
    vertex_set_init(&list1, graph->num_nodes);
    vertex_set *frontier = &list1;

    // initialize all nodes to NOT_VISITED
    #pragma omp parallel for
    for (int i = 0; i < graph->num_nodes; i++){
        frontier->vertices[i] = NOT_VISITED_MARKER;
        sol->distances[i] = NOT_VISITED_MARKER;
    }

    // setup frontier with the root node
    int depth = 1;
    frontier->vertices[frontier->count++] = depth;
    sol->distances[ROOT_NODE_ID] = 0;

    while (frontier->count != 0){

#ifdef VERBOSE
        double start_time = CycleTimer::currentSeconds();
#endif

        frontier->count = 0;
        top_down_step(graph, frontier, sol->distances, &depth);
        depth++;

#ifdef VERBOSE
        double end_time = CycleTimer::currentSeconds();
        printf("frontier=%-10d %.4f sec\n", frontier->count, end_time - start_time);
#endif

    }
}

void bottom_up_step(Graph g, vertex_set* frontier, int* distances, int* depth){
    #pragma omp parallel for
    for(int node = 0; node < g->num_nodes; ++node){
        if(frontier->vertices[node] == NOT_VISITED_MARKER){

            const Vertex* v_start = incoming_begin(g, node);
            const Vertex* v_end = incoming_end(g, node);
            for (const Vertex* v=v_start; v!=v_end; v++){
                if(frontier->vertices[*v] == *depth){
                    distances[node] = distances[*v] + 1;
                    frontier->vertices[node] = *depth + 1;
                    frontier->count++;
                    break;
                }
            }
        }
    }
}

void bfs_bottom_up(Graph graph, solution *sol){

    vertex_set list;
    vertex_set_init(&list, graph->num_nodes);
    vertex_set* frontier = &list;
    
    #pragma omp parallel for
    for(int i = 0; i < graph->num_nodes; ++i){
        frontier->vertices[i] = NOT_VISITED_MARKER;
        sol->distances[i] = NOT_VISITED_MARKER;
    }

    int depth = 1;
    frontier->vertices[frontier->count++] = depth;
    sol->distances[ROOT_NODE_ID] = 0;
    
    
    while(frontier->count != 0){
#ifdef VERBOSE
        double start_time = CycleTimer::currentSeconds();
#endif

        frontier->count = 0;
        bottom_up_step(graph, frontier, sol->distances, &depth);
        depth++;

#ifdef VERBOSE
        double end_time = CycleTimer::currentSeconds();
        printf("frontier=%-10d %.4f sec\n", frontier->count, end_time - start_time);
#endif

    }
    // For PP students:
    //
    // You will need to implement the "bottom up" BFS here as
    // described in the handout.
    //
    // As a result of your code's execution, sol.distances should be
    // correctly populated for all nodes in the graph.
    //
    // As was done in the top-down case, you may wish to organize your
    // code by creating subroutine bottom_up_step() that is called in
    // each step of the BFS process.
}

void bfs_hybrid(Graph graph, solution *sol){
    
    vertex_set list;
    vertex_set_init(&list, graph->num_nodes);
    vertex_set* frontier = &list;
    
    #pragma omp parallel for
    for(int i = 0; i < graph->num_nodes; ++i){
        frontier->vertices[i] = NOT_VISITED_MARKER;
        sol->distances[i] = NOT_VISITED_MARKER;
    }

    int depth = 1;
    frontier->vertices[frontier->count++] = depth;
    sol->distances[ROOT_NODE_ID] = 0;

    while(frontier->count != 0){

#ifdef VERBOSE
        double start_time = CycleTimer::currentSeconds();
#endif
        if(frontier->count >= graph->num_nodes/beta){
            frontier->count = 0;
            bottom_up_step(graph, frontier, sol->distances, &depth);
        }
        else{
            frontier->count = 0;
            top_down_step(graph, frontier, sol->distances, &depth);
        }
        depth++;

#ifdef VERBOSE
        double end_time = CycleTimer::currentSeconds();
        printf("frontier=%-10d %.4f sec\n", frontier->count, end_time - start_time);
#endif

    }
    
    // For PP students:
    //
    // You will need to implement the "hybrid" BFS here as
    // described in the handout.
}
