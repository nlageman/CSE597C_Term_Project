#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <igraph/igraph.h>

#include "graph.h"

static int comp_int(const void* a, const void* b){
  int A = *((int*)a), B=*((int*)b);
  return A-B;
}

static FILE* open_graph_file(char* filename){
  FILE *infp;
  infp = fopen(filename, "r");
  if (infp == NULL) {
    fprintf(stderr, "Error: could not open input graph file\n");
    fprintf(stderr, "Exiting ...\n");
    exit(1);
  }
  return infp;
}
static void get_dims(FILE* infp, int* n, int* m){
  char* linebuf= NULL;
  size_t readlen = 0;

  if(getline(&linebuf, &readlen, infp)==-1 ||
      sscanf(linebuf, "%d %d", n, m)==EOF) {
    fprintf(stderr, "Error: could not read dimensions of graph from file");
    fprintf(stderr, "Exiting ...\n");
    exit(1);
  }
  return;
}

int read_to_igraph(char *filename, igraph_t *ig){

  FILE * infp = open_graph_file(filename);
  char *linebuf = NULL;
  size_t readlen = 0;
  int u, v;

  int n, m;
  get_dims(infp, &n, &m);

  //Create igraph with n vertices and 0 edges
  igraph_empty(ig, n, IGRAPH_UNDIRECTED);

  //Read in edges
  igraph_vector_t edges;
  igraph_vector_init(&edges, 0);
  igraph_vector_reserve(&edges, 2*m);

  while (getline(&linebuf, &readlen, infp) != -1) {
    sscanf(linebuf, "%d %d", &u, &v);
    igraph_vector_push_back(&edges,u);
    igraph_vector_push_back(&edges,v);
  }
  igraph_add_edges(ig, &edges, 0);

  //Cleanup
  igraph_vector_destroy(&edges);

  return 0;

}

int read_graph_file(char *filename, graph_t *g)
{

  FILE* infp = open_graph_file(filename);

  char *linebuf = NULL;
  size_t readlen = 0;
  int u, v;

  int n, m;

  /* skip first line */
  get_dims(infp, &n, &m);

  fprintf(stderr, "Reading graph ... ");
  int *degrees;
  degrees = (int *) malloc(n*sizeof(int));
  assert(degrees != NULL);

  int i;
  for (i = 0; i<n; i++) {
    degrees[i] = 0;
  }

  /* read remaining lines */
  while (getline(&linebuf, &readlen, infp) != -1) {
    sscanf(linebuf, "%d %d", &u, &v);
    degrees[u]++;
    degrees[v]++;
  }
  fclose(infp);

  int degreeZero=0;
  for (int ii=0; ii<n; ii++){
    if(degrees[ii]==0) {degreeZero++;}
  }
  fprintf(stderr, "%d node(s) with degree zero.\n", degreeZero);

  int *num_edges;
  int *adj;

  num_edges = (int *) malloc((n+1) * sizeof(int));
  adj       = (int *) malloc(2*m * sizeof(int));
  assert(num_edges != NULL);
  assert(adj != NULL);

  num_edges[0] = 0;
  for (i=1; i<=n; i++) {
    num_edges[i] = num_edges[i-1] + degrees[i-1];
  }

  for (i=0; i<n; i++) {
    degrees[i] = 0;
  }

  /* Read file again to load graph */
  infp = fopen(filename, "r");
  /* skip first line */
  getline(&linebuf, &readlen, infp);

  /* read remaining lines */
  while (getline(&linebuf, &readlen, infp) != -1) {
    sscanf(linebuf, "%d %d", &u, &v);
    int uv_loc = num_edges[u] + degrees[u]++;
    int vu_loc = num_edges[v] + degrees[v]++;
    adj[uv_loc] = v;
    adj[vu_loc] = u;
  }
  fclose(infp);

  for(i=0; i<n; i++){
    qsort(&(adj[num_edges[i]]), num_edges[i+1]-num_edges[i],
        sizeof(int), comp_int);
  }

  if (linebuf)
    free(linebuf);
  free(degrees);

  g->n = n;
  g->m = m;
  g->num_edges = num_edges;
  g->adj = adj;

  return 0;
}

int read_vertex_names(char* vertex_ids_filename, char** v_names, int** name_offsets, int n){
  /* read vertex id names from file */
  FILE *infp;
  infp = fopen(vertex_ids_filename, "r");
  if (infp == NULL) {
    fprintf(stderr, "Error opening vertex id names file.\n"
        "Exiting ...\n");
    exit(1);
  }

  fseek(infp, 0L, SEEK_END);
  size_t names_file_size = ftell(infp);
  fseek(infp, 0L, SEEK_SET);

  char *names;
  names = (char *) malloc(names_file_size * sizeof(char));
  assert(names != NULL);
  fread(names, sizeof(char), names_file_size, infp);
  fclose(infp);

  int *vertname_offsets;
  vertname_offsets = (int *) malloc(n * sizeof(int));
  assert(vertname_offsets != NULL);
  vertname_offsets[0] = 0;
  int j = 1;
  for (int i=1; i<names_file_size-1; i++) {
    if (names[i] == '\n') {
      names[i] = '\0';
      vertname_offsets[j++] = i+1;
      continue;
    }
  }
  names[names_file_size-1] = '\0';
  assert(j == n);

  *name_offsets = vertname_offsets;
  *v_names = names;
  return 0;
}
