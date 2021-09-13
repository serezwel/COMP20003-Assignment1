#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include "read_input.h"
#include "create_polygon.h"

#define NUM_NEW_EDGES 3
#define NUM_NEW_VERTICES 2
#define NUM_NEW_FACES 1
/* Defining the DCEL struct to store all vertices, edges and faces */
typedef struct dcel {
    Vertex_t *vertices;
    Edge_t **edges;
    Face_t **faces;
} dcel_t;
/* Function to search for the midpoint of an edge */
Vertex_t find_midpoint(Edge_t *edge, Vertex_t *vertices) {
    Vertex_t vertex1, vertex2, end_vertex;
    vertex1 = vertices[edge->start_vertex];
    vertex2 = vertices[edge->end_vertex];
    end_vertex = (Vertex_t) { .xVertex = (vertex1.xVertex +
                              vertex2.xVertex) / 2,
                              .yVertex = (vertex1.yVertex +
                                      vertex2.yVertex) / 2};
    return end_vertex;
}
/* Function to find the average point for the half edge detection to split */
Vertex_t find_average(Vertex_t v1, Vertex_t v2) {
    Vertex_t test_v = (Vertex_t) { .xVertex = (v1.xVertex + v2.xVertex) / 2,
                                   .yVertex = (v1.yVertex + v2.yVertex) / 2};
    return test_v;
}
/* Function to add new vertices into the vertices array */
Vertex_t *add_vertices(Vertex_t *vertices, Edge_t **edges, int *num_vertices,
                     int *vertices_size, int mid1, int mid2) {
    /* Update vertices array size if number of vertices exceeds the amount */
    if (*num_vertices == *vertices_size) {
        *vertices_size *= 2;
        vertices = realloc(vertices, sizeof(Vertex_t) * (*vertices_size));
        assert(vertices);
    }
    /* vertices[(*num_vertices)] stores the start vertex of the splitting edge
     * vertices[(*num_vertices) + 1]
     * stores the end vertex of the splitting edge */
    vertices[(*num_vertices)] = find_midpoint(edges[mid1], vertices);
    vertices[(*num_vertices) + 1] = find_midpoint(edges[mid2], vertices);
    *num_vertices += NUM_NEW_VERTICES;
    return vertices;
}
/* Function to check whether a
 * point is inside the correct half-plane of an edge */
bool point_inside(Edge_t *edge, Vertex_t *vertices, Vertex_t checked) {
    double gradient, c, y_predict, y_result;
    double x1 = vertices[edge->start_vertex].xVertex;
    double y1 = vertices[edge->start_vertex].yVertex;
    double x2 = vertices[edge->end_vertex].xVertex;
    double y2 = vertices[edge->end_vertex].yVertex;
    if (x1 == x2) {
        if (y1 < y2) {
            if (checked.xVertex > x2) {
                return true;
            } else {
                return false;
            }
        } else {
            if (checked.xVertex < x2) {
                return true;
            } else {
                return false;
            }
        }
    } else {
        gradient = (y2 - y1)/(x2 - x1);
        c = y2 - (gradient * x2);
        y_predict = (gradient * checked.xVertex) + c;
        y_result = checked.yVertex - y_predict;
        if (x1 > x2) {
            if (y_result >= 0) {
                return true;
            } else {
                return false;
            }
        } else {
            if (y_result <= 0) {
                return true;
            } else {
                return false;
            }
        }
    }
}
/* Function to cut the edges that are used to split into half */
void cut_edges(Edge_t *edge1, Edge_t *edge2,
               int num_vertices, int *old_vertex1, int *old_vertex2) {
    *old_vertex1 = edge1->end_vertex;
    *old_vertex2 = edge2->start_vertex;
    edge1->end_vertex = num_vertices - 2;
    if (edge1->other_half_edge != NULL) {
        edge1->other_half_edge->start_vertex = num_vertices - 2;
    }
    edge2->start_vertex = num_vertices - 1;
    if (edge2->other_half_edge != NULL) {
        edge2->other_half_edge->end_vertex = num_vertices - 1;
    }
}
/* Function to create the joining half edge of the split and its pair half edge */
Edge_t **create_joining_half_edge(Edge_t **edges, int num1, int num2
                                  , int num_vertices, int num_edges) {
    edges[num_edges] = (Half_edge_t *)malloc(sizeof(Half_edge_t));
    edges[num_edges]->other_half_edge =
            (Half_edge_t *)malloc(sizeof(Half_edge_t));
    edges[num_edges]->start_vertex = num_vertices - 2;
    edges[num_edges]->end_vertex = num_vertices - 1;
    edges[num_edges]->prev_half_edge = edges[num1];
    edges[num1]->next_half_edge = edges[num_edges];
    edges[num_edges]->next_half_edge = edges[num2];
    edges[num2]->prev_half_edge = edges[num_edges];
    edges[num_edges]->edge = num_edges;
    edges[num_edges]->face = edges[num1]->face;
    edges[num_edges]->other_half_edge->end_vertex = num_vertices - 2;
    edges[num_edges]->other_half_edge->start_vertex = num_vertices - 1;
    edges[num_edges]->other_half_edge->other_half_edge = edges[num_edges];
    return edges;
}
/* Function to update the pointers to
 * next and previous half edge of the split edges */
void update_old_edge(Edge_t **edges, Edge_t *edge1,
                     Edge_t *edge2, int num_edges) {
    edge1->next_half_edge = edges[num_edges];
    edge2->prev_half_edge = edges[num_edges];
}
/* Function to create the half edges and its pair
 * half edge (if it exists) of the remaining half that got cut
 * because of the split as well as updating
 * the joining half edge facing the new face */
Edge_t **create_new_face_edges(Edge_t **edges, int num_edges, int num_faces
                               , int old_vertex1, int old_vertex2,
                               Half_edge_t *save1, Half_edge_t *save2) {
    edges[num_edges + 1] = (Half_edge_t *)malloc(sizeof(Half_edge_t));
    assert(edges[num_edges + 1]);
    edges[num_edges + 2] = (Half_edge_t *)malloc(sizeof(Half_edge_t));
    assert(edges[num_edges + 2]);
    edges[num_edges + 1]->start_vertex = edges[num_edges]->start_vertex;
    edges[num_edges + 1]->end_vertex = old_vertex1;
    edges[num_edges + 1]->prev_half_edge = edges[num_edges]->other_half_edge;
    edges[num_edges + 1]->other_half_edge = NULL;
    edges[num_edges + 1]->edge = num_edges + 1;
    if (save1 != NULL) {
        edges[num_edges + 1]->next_half_edge = save1;
    } else {
        edges[num_edges + 1]->next_half_edge = edges[num_edges + 2];
    }
    edges[num_edges + 1]->next_half_edge->prev_half_edge = edges[num_edges + 1];
    edges[num_edges + 2]->end_vertex = edges[num_edges]->end_vertex;
    edges[num_edges + 2]->start_vertex = old_vertex2;
    edges[num_edges + 2]->next_half_edge = edges[num_edges]->other_half_edge;
    edges[num_edges + 2]->other_half_edge = NULL;
    edges[num_edges + 2]->edge = num_edges + 2;
    if (save2 != NULL) {
        edges[num_edges + 2]->prev_half_edge = save2;
    } else {
        edges[num_edges + 2]->prev_half_edge = edges[num_edges + 1];
    }
    edges[num_edges + 2]->prev_half_edge->next_half_edge = edges[num_edges + 2];
    edges[num_edges]->other_half_edge->face = num_faces;
    edges[num_edges]->other_half_edge->next_half_edge = edges[num_edges + 1];
    edges[num_edges]->other_half_edge->prev_half_edge = edges[num_edges + 2];
    edges[num_edges]->other_half_edge->edge = num_edges;
    return edges;
}
/* Function to create the pair half edge of the 2
 * non-joining new edges created from the split if
 * the pair half edge of the split edges exist */
Edge_t **change_other_half_edge(Edge_t **edges,
                                Edge_t *edge1, Edge_t *edge2, int num_edges) {
    if (edge1->other_half_edge != NULL) {
        edges[num_edges + 1]->other_half_edge =
                (Half_edge_t *)malloc(sizeof(Half_edge_t));
        assert(edges[num_edges + 1]->other_half_edge);
        edges[num_edges + 1]->other_half_edge->start_vertex =
                edges[num_edges + 1]->end_vertex;
        edges[num_edges + 1]->other_half_edge->end_vertex =
                edges[num_edges + 1]->start_vertex;
        edges[num_edges + 1]->other_half_edge->prev_half_edge =
                edge1->other_half_edge->prev_half_edge;
        edges[num_edges + 1]->other_half_edge->next_half_edge =
                edge1->other_half_edge;
        edge1->other_half_edge->prev_half_edge =
                edges[num_edges + 1]->other_half_edge;
        edges[num_edges + 1]->other_half_edge->other_half_edge =
                edges[num_edges + 1];
        edges[num_edges + 1]->other_half_edge->edge =
                num_edges + 1;
        edges[num_edges + 1]->other_half_edge->face =
                edge1->other_half_edge->face;
        edge1->other_half_edge->prev_half_edge =
                edges[num_edges + 1]->other_half_edge;
    }
    if (edge2->other_half_edge != NULL) {
        edges[num_edges + 2]->other_half_edge =
                (Half_edge_t *)malloc(sizeof(Half_edge_t));
        assert(edges[num_edges + 2]->other_half_edge);
        edges[num_edges + 2]->other_half_edge->start_vertex =
                edges[num_edges + 2]->end_vertex;
        edges[num_edges + 2]->other_half_edge->end_vertex =
                edges[num_edges + 2]->start_vertex;
        edges[num_edges + 2]->other_half_edge->prev_half_edge =
                edge2->other_half_edge;
        edges[num_edges + 2]->other_half_edge->next_half_edge =
                edge2->other_half_edge->next_half_edge;
        edge2->other_half_edge->next_half_edge =
                edges[num_edges + 2]->other_half_edge;
        edges[num_edges + 2]->other_half_edge->other_half_edge =
                edges[num_edges + 2];
        edges[num_edges + 2]->other_half_edge->edge =
                num_edges + 2;
        edges[num_edges + 2]->other_half_edge->face =
                edge2->other_half_edge->face;
        edge2->other_half_edge->prev_half_edge =
                edges[num_edges + 2]->other_half_edge;
    }
    return edges;
}
/* Function to perform the splitting using the above helper functions */
dcel_t splitting(dcel_t dcel, int num1, int num2, int *num_vertices
                  , int *vertices_size, int *num_edges, int *num_faces) {
    int old_vertex1 = 0, old_vertex2 = 0, cut_face;
    Half_edge_t *save1 = NULL, *save2 = NULL, *start = NULL, *cur = NULL;
    /* Allow more space for 3 new edges and 1 new face */
    dcel.edges = realloc(dcel.edges,
                         sizeof(Edge_t *) * ((*num_edges) + NUM_NEW_EDGES));
    assert(dcel.edges);
    dcel.faces = realloc(dcel.faces,
                         sizeof(Face_t *) * ((*num_faces) + NUM_NEW_FACES));
    assert(dcel.faces);
    /* Add the new vertices first */
    dcel.vertices = add_vertices(dcel.vertices, dcel.edges,
                                 num_vertices, vertices_size, num1, num2);
    Vertex_t midpoint = find_average(dcel.vertices[(*num_vertices) - 2],
                                     dcel.vertices[(*num_vertices) - 1]);
    /* Check for which half edge to use in the splitting edges*/
    if (point_inside(dcel.edges[num1], dcel.vertices, midpoint) == false) {
        dcel.edges[num1] = dcel.edges[num1]->other_half_edge;
    }
    if (point_inside(dcel.edges[num2], dcel.vertices, midpoint) == false) {
        dcel.edges[num2] = dcel.edges[num2]->other_half_edge;
    }
    /* Store half edges prev and next pointer in
     * case it is not the splitting half edges */
    if (dcel.edges[num1]->next_half_edge != dcel.edges[num2]) {
        save1 = dcel.edges[num1]->next_half_edge;
    }
    if (dcel.edges[num2]->prev_half_edge != dcel.edges[num1]) {
        save2 = dcel.edges[num2]->prev_half_edge;
    }
    /* Perform the split and updating the old face
     * pointers so that it loops through the old face */
    cut_edges(dcel.edges[num1], dcel.edges[num2],
              *num_vertices, &old_vertex1, &old_vertex2);
    update_old_edge(dcel.edges, dcel.edges[num1], dcel.edges[num2], *num_edges);
    dcel.edges = create_joining_half_edge(dcel.edges,num1, num2,
                                          *num_vertices, *num_edges);
    cut_face = dcel.edges[*num_edges]->face;
    dcel.faces[cut_face] = dcel.edges[*num_edges];
    /* Update the half edges of the old face pre-split */
    start = dcel.edges[(*num_edges)];
    cur = start->next_half_edge;
    while (cur != start) {
        cur->face = start->face;
        cur = cur->next_half_edge;
    }
    /* Create the new face and edges corresponding to the face
     * as well updating pointers so that it loops through
     * the new face */
    dcel.edges = create_new_face_edges(dcel.edges, *num_edges,*num_faces,
                                       old_vertex1, old_vertex2, save1, save2);
    dcel.faces[*num_faces] = dcel.edges[(*num_edges)]->other_half_edge;
    dcel.edges = change_other_half_edge(dcel.edges, dcel.edges[num1],
                                        dcel.edges[num2], *num_edges);
    /* Update the face index of the new half edges facing the new face */
    start = dcel.edges[(*num_edges)]->other_half_edge;
    cur = start->next_half_edge;
    while (cur != start) {
        cur->face = start->face;
        cur = cur->next_half_edge;
    }
    /* Update the number of edges and faces in the current graph */
    *num_edges += NUM_NEW_EDGES;
    *num_faces += NUM_NEW_FACES;
    return dcel;
}
