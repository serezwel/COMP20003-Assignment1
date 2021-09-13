#ifndef ASSIGNMENT1_READ_INPUT_H
#define ASSIGNMENT1_READ_INPUT_H
/* The header file contains definition of
 * structs and function prototypes to read and free watchtowers
 * and vertices of the graph */

typedef struct Vertex{
    double xVertex;
    double yVertex;
} Vertex_t;

typedef struct Watchtower{ /*Defining Watchtower*/
    char *watchtower_ID;
    char *postcode;
    int population;
    char *contact_name;
    Vertex_t watchtower_vertex;
} Watchtower_t;

Watchtower_t **read_csv(char *, int *);
void free_csv(Watchtower_t **, int);
Vertex_t *read_polygon(char *, int *, int *);
void free_polygon(Vertex_t *);
#endif //ASSIGNMENT1_READ_INPUT_H
