#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "read_input.h"
#include "create_polygon.h"
#include "dcel.h"
int main(int argc, char **argv) {
    char *csv_filename = argv[1];
    char *polygon_filename = argv[2];
    int i, j, num_vertices = 0, num_edges = 0;
    int num_faces = 0, vertices_size = 0, edge1, edge2;
    int num_watchtowers = 0;
    bool is_in_face = true;
    /* Initialization of vertices, watchtowers,
     * edges and faces as well as updating the number of
     * elements inside the array and the array size */
    Vertex_t *vertices = read_polygon(polygon_filename,
                                      &num_vertices, &vertices_size);
    Watchtower_t **watchtowers = read_csv(csv_filename,
                                          &num_watchtowers);
    Edge_t **edges = initial_edges(num_vertices, &num_edges);
    Face_t **faces = initial_faces(edges, &num_faces);
    Half_edge_t *start = NULL, *cur = NULL;
    dcel_t dcel = { .vertices = vertices,
                    .edges = edges,
                    .faces = faces};
    /* While loop scans for edge input to perform split
     * and performs the split for each input */
    while (scanf("%d %d", &edge1, &edge2) == 2) {
        dcel = splitting(dcel, edge1, edge2, &num_vertices,
                         &vertices_size, &num_edges, &num_faces);
    }
    FILE *output_file = fopen(argv[3], "w");
    int sum[num_faces], sum_current;
    /* Loop through each face, as well as
     * looping through watchtowers in each face to determine which watchtowers
     * are inside each face, writing the information
     * of all watchtowers in each face */
    for (i = 0; i < num_faces; i++) {
        fprintf(output_file, "%d\n", i);
        sum_current = 0;
        for (j = 0; j < num_watchtowers; j++) {
            is_in_face = true;
            start = dcel.faces[i];
            cur = start->next_half_edge;
            if (point_inside(start, dcel.vertices,
                             watchtowers[j]->watchtower_vertex) == false) {
                is_in_face = false;
            }
            while (cur != start) {
                if (point_inside(cur, dcel.vertices,
                                 watchtowers[j]->watchtower_vertex) == false) {
                    is_in_face = false;
                    break;
                }
                cur = cur->next_half_edge;
            }
            if (is_in_face == true) {
                fprintf(output_file, "Watchtower ID: %s, Postcode: %s,"
                                     " Population Served: %d, "
                                     "Watchtower Point of Contact Name: %s,"
                                     " x: %lf, y: %lf\n",
                                     watchtowers[j]->watchtower_ID,
                                     watchtowers[j]->postcode,
                                     watchtowers[j]->population,
                                     watchtowers[j]->contact_name,
                                     watchtowers[j]->watchtower_vertex.xVertex,
                                     watchtowers[j]->watchtower_vertex.yVertex);
                sum_current += watchtowers[j]->population;
            }
        }
        sum[i] = sum_current;
    }
    /* Writes the total population
     * (sum of population of watchtowers) in each face */
    for (i = 0; i < num_faces; i++) {
        fprintf(output_file, "Face %d population served: %d\n",
                i, sum[i]);
    }
    fclose(output_file);
    free_edges(dcel.edges, num_edges);
    free_csv(watchtowers, num_watchtowers);
    free_polygon(dcel.vertices);
    free(dcel.faces);
    return 0;
}

