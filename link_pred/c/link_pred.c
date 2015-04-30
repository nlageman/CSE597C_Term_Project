#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "graph.h"
#include "community_metrics.h"

#define SWAP_INTP(a,b) {int * t3mp; t3mp=(a); (a)=(b); (b)=t3mp;}
#define MAX_ITERS 100

// Structure and requisite comparison function //
typedef struct score_t{
  int edge;
  double score;
} score_t;

int score_cmp(const void* a, const void* b){
  score_t* A = (score_t*)a;
  score_t* B = (score_t*)b;
  return (int)(B->score - A->score);
}

///////////////////////////////////////////////////////////////////////////////
//////////////////////////// BEGIN ADAMIC /////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
score_t* calc_adamic(graph_t* g){
  score_t* edges   = calloc(g->n*g->n, sizeof(score_t));
  double* t_scores = calloc(g->n, sizeof(double));
  int* visited = calloc(g->n, sizeof(int));
  int i,j,k,x,y,z,numVisited,n=g->n;
  int numE=0;
  double t_score=0;
  int* has_edge = calloc(n, sizeof(int));

  for(x = 0; x < n; x++){
    numVisited=0;

    for(k = g->num_edges[x]; k < g->num_edges[x+1]; k++){
      z=g->adj[k];
      t_score = 1.0/log(g->num_edges[z+1]-g->num_edges[z]);
      for(j = g->num_edges[z]; j < g->num_edges[z+1]; j++){
        y = g->adj[j];
        if(y!=x && t_score!=0 && t_scores[y]==0) {
          visited[numVisited++]=y;
        }
        t_scores[y] += t_score;
      }
    }

    for(k = g->num_edges[x]; k < g->num_edges[x+1]; k++){
      has_edge[g->adj[k]] = 1;
    }

    //For each edge visited we now have calculated it's complete score so store
    //in the scores array
    for(i=0; i<numVisited; i++){
      y=visited[i];
      if(t_scores[y]==0 || has_edge[y]) continue;

      edges[numE++].edge = x*n + y;
      edges[numE].score = t_scores[y];

      //Clear data
      visited[i] = 0;
      t_scores[y] = 0;
    }

    for(k = g->num_edges[x]; k < g->num_edges[x+1]; k++){
      has_edge[g->adj[k]] = 0;
    }
  }

  qsort(edges, numE, sizeof(score_t), score_cmp);

  free(t_scores);
  free(visited);
  return edges;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////// BEGIN KATZ //////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

int *list_to_matrix(graph_t *g) {
  int n = g->n;
  int *Adj = calloc(n*n, sizeof(int));
  int i, j;

  for (i=0; i < n; i++) {
    for (j=g->num_edges[i]; j < g->num_edges[i+1]; j++) {
      int u = g->adj[j];
      Adj[i*n + u] = 1;
    }
  }
  return Adj;
}

int *matrix_mult(int *A, int *B, int n) {
  int i, j, k;
  int *Adj = calloc(n*n, sizeof(int));
  for (i=0; i < n; i++) {
    for (j=0; j < n; j++) {
      int sum = 0;
      for (k=0; k < n; k++) {
        sum = sum + (A[i*n+k]*B[k*n+j]);
      }
      Adj[i*n+j] = sum;
    }
  }
  return Adj;
}

score_t *katz_it(graph_t *g, double beta) {
  int n = g->n;
  int i, j, k;
  double exp = beta;
  int* Adj = list_to_matrix(g);
  int* cpy = malloc(n*n*sizeof(int));
  int* temp = cpy;
  memcpy(cpy, Adj, n*n*sizeof(int));
  //initialize scores
  score_t *scores = calloc(n*n, sizeof(score_t));
  for (i = 0; i < n; i++) {
    for (j = 0; j < n; j++) {
      scores[i*n + j].edge = i*n + j;
    }
  }
  // count paths of length 1 through 10
  for(i = 0; i < 10; i++) {
    fprintf(stderr, "Iter: %d\n", i);
    for (j = 0; j < n; j++) {
      for (k = 0; k < n; k++) {
        scores[j*n + k].score += temp[j*n + k] * exp;
      }
    }
    exp = exp * beta;
    temp = matrix_mult(Adj, cpy, n);
    free(cpy);
    cpy = temp;
  }

  //Filter out edges in existence
  for(i = 0; i < n; i++){
    scores[i*(n+1)].score = 0;
    for(j = g->num_edges[i]; j < g->num_edges[i+1]; j++){
      scores[i*n + g->adj[j]].score = 0;
    }
  }
  // sort the scores
  qsort(scores, n*n, sizeof(score_t), score_cmp);

  free(Adj);
  Adj = NULL;
  free(temp);
  temp = NULL;
  cpy = NULL;

  return scores;
}

//matrix_mult(graph_t* a, )
int main(int argc, char **argv) {

  char *input_graph_filename;
  char* vertex_ids_filename;
  char* v_names = 0x0;
  int* name_offsets = 0x0;
  graph_t g;
  char* files[4] = {"results/adamic_10.dat", "results/adamic_100.dat", "results/katz_10.dat", "results/katz_100.dat"};

  /* parse input args */
  if (argc != 4) {
    fprintf(stderr, "%s <edge list file> <vertex ids file> <beta>\n",
        argv[0]);
    exit(1);
  }


  double beta = atof(argv[3]);

  input_graph_filename = argv[1];
  vertex_ids_filename  = argv[2];

  read_graph_file(input_graph_filename, &g);
  read_vertex_names(vertex_ids_filename, &v_names, &name_offsets, g.n);

  score_t* links = calc_adamic(&g);

  ////////// Begin print code (ugly don't look) /////////////////

  FILE* f;
  f = fopen(files[0], "w");
  for(int i=0; i<10; i++){
    fprintf(f,"Edge: (%s,%s); Score: %f\n",
        v_names+name_offsets[links[i].edge/g.n],
        v_names+name_offsets[links[i].edge%g.n], links[i].score);
  }
  fclose(f);
  f = fopen(files[1], "w");
  for(int i=0; i<100; i++){
    fprintf(f,"Edge: (%s,%s); Score: %f\n",
        v_names+name_offsets[links[i].edge/g.n],
        v_names+name_offsets[links[i].edge%g.n], links[i].score);
  }
  // Give the probability for edges to not be connected to neighbors
  score_t *t;
  t = katz_it(&g, beta);
  fprintf(stderr, "The top predicted links and their scores:\n");
  for (int i = 0; i < 10; i++) {
    fprintf(stderr, "Edge: (%d,%d); Score: %f\n",
        t[i].edge/g.n, t[i].edge%g.n, t[i].score);
  }

  f = fopen(files[2], "w");
  for(int i=0; i<10; i++){
    fprintf(f,"Edge: (%s,%s); Score: %f\n",
        v_names+name_offsets[t[i].edge/g.n],
        v_names+name_offsets[t[i].edge%g.n], t[i].score);
  }
  fclose(f);

  f = fopen(files[3], "w");
  for(int i=0; i<100; i++){
    fprintf(f,"Edge: (%s,%s); Score: %f\n",
        v_names+name_offsets[t[i].edge/g.n],
        v_names+name_offsets[t[i].edge%g.n], t[i].score);
  }
  fclose(f);

  free(v_names);
  free(name_offsets);
  return 0;
}
