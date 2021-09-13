#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define FIELD_SIZE 128
#define INITIAL_SIZE 1
#define BUFFER_SIZE 0
#define GROW_SIZE 2

/* Defining Vertices */
typedef struct Vertex{
    double xVertex;
    double yVertex;
} Vertex_t;
/* Defining Watchtower */
typedef struct Watchtower{
    char *watchtower_ID;
    char *postcode;
    int population;
    char *contact_name;
    Vertex_t watchtower_vertex;
} Watchtower_t;
/* Defining half edge struct*/


/* Function to read CSV file */
Watchtower_t **read_csv(char *filename, int *num_watchtowers) {
    /* Dynamic memory allocation for 1 watchtower */
    Watchtower_t **watchtowers = (Watchtower_t **)malloc(sizeof(Watchtower_t*));
    assert(watchtowers);
    /* String delimiter "comma" to separate the comma-separated values */
    const char comma[2] = ",";
    double x, y;
    int i = 0, arr_size = INITIAL_SIZE, j;
    size_t lineBufferLength = BUFFER_SIZE;
    char *line = NULL, *token;
    FILE *csv_file = fopen(filename, "r");
    /* Throw out the header line */
    getline(&line, &lineBufferLength, csv_file);
    /* Reading the CSV file until getline reaches EOF */
    while(getline(&line, &lineBufferLength, csv_file) != EOF) {
        /* Set index at the end of string to
         * avoid any access to uninitialized memory */
        /* If block in case number of
         * watchtowers exceeds the memory allocated */
        /* Exponential additional memory,
         * increase size of array by multiple of 2 each time */
        if (i == arr_size) {
            arr_size *= GROW_SIZE;
            watchtowers = realloc(watchtowers,
                                  sizeof(Watchtower_t *) * (arr_size));
            assert(watchtowers);
            for (j = (arr_size / 2); j < arr_size; j++) {
                watchtowers[j] = NULL;
            }
        }
        watchtowers[i] = (Watchtower_t *)malloc(sizeof(Watchtower_t));
        assert(watchtowers[i]);
        /* strtok is called each time to move to the next column in the row
         * and store it into the corresponding variable*/
        token = strtok(line, comma);
        watchtowers[i]->watchtower_ID = (char *)calloc(FIELD_SIZE + 1,
                                                       sizeof(char));
        assert(watchtowers[i]->watchtower_ID);
        strcpy(watchtowers[i]->watchtower_ID, token);
        token = strtok(NULL, comma);
        watchtowers[i]->postcode = (char *)calloc(FIELD_SIZE + 1,
                                                  sizeof(char));
        assert(watchtowers[i]->postcode);
        strcpy(watchtowers[i]->postcode, token);
        token = strtok(NULL, comma);
        watchtowers[i]->population = atoi(token);
        token = strtok(NULL, comma);
        watchtowers[i]->contact_name = (char *)calloc(FIELD_SIZE + 1,
                                                      sizeof(char));
        assert(watchtowers[i]->contact_name);
        strcpy(watchtowers[i]->contact_name, token);
        token = strtok(NULL, comma);
        x = strtod(token, NULL);
        token = strtok(NULL, comma);
        y = strtod(token, NULL);
        watchtowers[i]->watchtower_vertex = (Vertex_t) { .xVertex = x,
                                                         .yVertex = y };
        i++;
        (*num_watchtowers) += 1;
    }
    free(line);
    fclose(csv_file);
    return watchtowers;
}

/* Function to read the file containing polygon vertices */
Vertex_t *read_polygon(char *filename, int *num_vertices, int *vertices_size) {
    /* Similar to CSV, memory for vertices are dynamically allocated */
    Vertex_t *vertices = (Vertex_t *)malloc(sizeof(Vertex_t ));
    assert(vertices);
    /* Space character delimiter to separate x and y coordinates */
    const char space[2] = " ";
    char *line = NULL, *token;
    size_t lineBufferLength = BUFFER_SIZE;
    int i = 0, arr_size = INITIAL_SIZE;
    double x, y;
    FILE *polygon_file = fopen(filename, "r");
    while (getline(&line, &lineBufferLength, polygon_file) != EOF) {
        /* Set index at the end of string to avoid any
         * access to uninitialized memory */
        /* Similar to CSV, if original size is not
         * enough to store all vertices, realloc for more memory */
        /* Also increases exponentially */
        if (i == arr_size) {
            arr_size *= GROW_SIZE;
            vertices = realloc(vertices, sizeof(Vertex_t) * (arr_size));
            assert(vertices);
        }
        /* Read each x and y coordinates for the input vertices */
        token = strtok(line, space);
        x = strtod(token, NULL);
        token = strtok(NULL, space);
        y = strtod(token, NULL);
        vertices[i] = (Vertex_t) { .xVertex = x, .yVertex = y };
        i++;
    }
    /* Update vertices array size and number of vertices read */
    *vertices_size = arr_size;
    *num_vertices = i;
    free(line);
    fclose(polygon_file);
    return vertices;
}

/* This function frees all dynamically allocated memory
 * in the array of pointers to the watchtower */
void free_csv(Watchtower_t **watchtowers, int num_watchtowers) {
    int i;
    /* Free all dynamic memory for each watchtower before the array */
    for (i = 0; i < num_watchtowers; i++) {
        free(watchtowers[i]->watchtower_ID);
        free(watchtowers[i]->postcode);
        free(watchtowers[i]->contact_name);
        free(watchtowers[i]);
    }
    free(watchtowers);
}

/* This function frees all the memory that was
 * dynamically allocated to the vertices */
void free_polygon(Vertex_t *vertices) {
    /* Free all dynamic memory for each vertex before the array */
    free(vertices);
}
