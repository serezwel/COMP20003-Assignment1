
#ifndef ASSIGNMENT1_CREATE_POLYGON_H
#define ASSIGNMENT1_CREATE_POLYGON_H
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

Edge_t **initial_edges(int, int *);
Face_t **initial_faces(Edge_t **, int *);
void free_edges(Edge_t **, int);
#endif //ASSIGNMENT1_CREATE_POLYGON_H
