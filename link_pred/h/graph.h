#ifndef GRAPH_H
#define GRAPH_H
#include <igraph/igraph.h>
typedef struct {
  int n;          /* number of vertices */
  int m;          /* number of edges */
  int *adj;       /* adj[num_edges[i]..num_edges[i+1]-1] gives
                     vertices adjacent to vertex i */
  int *num_edges; /* num_edges[i+1]-num_edges[i] gives degree of
                     vertex i */
} graph_t;

int read_graph_file(char *filename, graph_t *g);
int read_to_igraph(char *filename, igraph_t *ig);
int read_vertex_names(char* vertex_ids_filename, char** v_names, int** name_offsets, int n);

#endif
