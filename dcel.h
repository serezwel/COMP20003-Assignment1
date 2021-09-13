#ifndef ASSIGNMENT1_DCEL_H
#define ASSIGNMENT1_DCEL_H
#include <stdbool.h>

typedef struct dcel {
    Vertex_t *vertices;
    Edge_t **edges;
    Face_t **faces;
} dcel_t;

Vertex_t find_midpoint(Edge_t *, Vertex_t *);
Vertex_t find_average(Vertex_t, Vertex_t);
Vertex_t *add_vertices(Vertex_t *, Edge_t **, int *, int *, int, int);
bool point_inside(Edge_t *, Vertex_t *, Vertex_t);
void cut_edges(Edge_t *, Edge_t *, int, int *, int *);
Edge_t **create_joining_half_edge(Edge_t **, int, int, int, int);
void update_old_edge(Edge_t **, Edge_t *, Edge_t *, int);
Edge_t **create_new_face_edges(Edge_t **, int, int, int, int,
                               Half_edge_t *, Half_edge_t *);
Edge_t **change_other_half_edge(Edge_t **, Edge_t *, Edge_t *, int);
dcel_t splitting(dcel_t, int, int, int *, int *, int *, int *);

#endif //ASSIGNMENT1_DCEL_H
