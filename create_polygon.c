#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "read_input.h"
typedef struct Half_edge {
    int start_vertex;
    int end_vertex;
    struct Half_edge *next_half_edge;
    struct Half_edge *prev_half_edge;
    struct Half_edge *other_half_edge;
    int face;
    int edge;
} Half_edge_t;

typedef Half_edge_t Edge_t;
typedef Half_edge_t Face_t;

Edge_t **initial_edges(int num_vertices, int *num_edges) {
    int i;
    Edge_t **edges = (Edge_t **)malloc(sizeof(Edge_t*) * num_vertices);
    assert(edges);
    for (i = 0; i < num_vertices; i++) {
        edges[i] = (Edge_t *)malloc(sizeof(Half_edge_t));
        assert(edges[i]);
    }
    for (i = 0; i < num_vertices; i++) {
        if (i == (num_vertices - 1)) {
            edges[i]->start_vertex = i;
            edges[i]->end_vertex = 0;
            edges[i]->next_half_edge = edges[0];
            edges[i]->prev_half_edge = edges[i - 1];
        } else if (i == 0) {
            edges[i]->start_vertex = i;
            edges[i]->end_vertex = i+1;
            edges[i]->next_half_edge = edges[i + 1];
            edges[i]->prev_half_edge = edges[num_vertices - 1];
        } else {
            edges[i]->start_vertex = i;
            edges[i]->end_vertex = i+1;
            edges[i]->next_half_edge = edges[i + 1];
            edges[i]->prev_half_edge = edges[i - 1];
        }
        edges[i]->other_half_edge = NULL;
        edges[i]->face = 0;
        edges[i]->edge = i;
    }
    *num_edges = i;
    return edges;
}

Face_t **initial_faces(Edge_t **edges, int *num_faces) {
    Face_t **faces = malloc(sizeof(Face_t *));
    faces[0] = edges[0];
    *num_faces = 1;
    return faces;
}

void free_edges(Edge_t **edges, int num_edges) {
    int i = 0;
    while (i < num_edges) {
        if (edges[i]) {
            free(edges[i]->other_half_edge);
            free(edges[i]);
            i++;
        }
    }
    free(edges);
}
