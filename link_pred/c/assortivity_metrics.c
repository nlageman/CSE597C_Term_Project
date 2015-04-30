#include <stdlib.h>
#include <assert.h>
#include <omp.h>

#include "graph.h"
#include "assortivity_metrics.h"
#define NUM_THREADS 4
/*
   int count_triangle_e(graph_t* g){
//For each edge <u,v>
//  adj_intersect = Adj(u) \cap  Adj(v)
//  For each w \in adj_intersect
//    output {u,v,w}; count++
//    Note each triangle will be outputted/counted 3 times (remove u adj list as loop through maybe? aka <a,b> is valid edge iff a<b (no self loops))
//    O(m*d_{max}*n)
return 0;
}
*/
static int edge_intersect_cnt(int* adj1, int len1, int* adj2, int len2){
  int v_i1=0, v_i2=0, count=0;
  int v_1=adj1[v_i1], v_2 = adj2[v_i2];
  while(v_i1<len1 && v_i2<len2){
    v_1 = adj1[v_i1];
    v_2 = adj2[v_i2];
    if(v_1<v_2){ v_i1++;}
    else if(v_1==v_2) { count++; v_i1++; v_i2++;}
    else { v_i2++; }
  }
  return count;
}
double* local_cluster_coeffs(graph_t* g){
  //For each node u
  //  For each neighbor v
  //    neigh_edges+=|Adj(u)\cap Adj(v)|
  //O(n*deg_{max}^2)?
  double* cluster_coeffs = malloc(g->n*sizeof(double));
#pragma omp parallel num_threads(NUM_THREADS)
  {
    int deg_v, neigh_edges, u, deg_u, count;

    int tId = omp_get_thread_num();
    int loopFrom=g->n/NUM_THREADS*tId;
    int loopTo=(tId==NUM_THREADS-1) ? g->n : g->n/NUM_THREADS*(tId+1);

    for(int v = loopFrom; v<loopTo; v++){
      deg_v = DEG(g,v);
      neigh_edges=0;
      for(int u_i=g->num_edges[v]; u_i<g->num_edges[v+1]; u_i++){
        u=g->adj[u_i];
        deg_u = DEG(g,u);
        count = edge_intersect_cnt(&g->adj[g->num_edges[u]], deg_u,
            &g->adj[g->num_edges[v]], deg_v);
        neigh_edges+=count;
      }
      if(deg_v!=0 && deg_v-1!=0)
        cluster_coeffs[v]=((double)neigh_edges)/(deg_v*(deg_v-1));
    }
  }
  return cluster_coeffs;
}

#define CNT_IND(v) ((v)*2+1)
#define E_IND(v) ((v)*2)
double calc_assort(graph_t* g, double* avg_neigh_deg){
  //Find max degree so we can  alloc. an array of that size*2 for e_{ii}'s' and a count of verts with each deg
  int max_deg=0;
  for(int v=0; v<g->n; v++){
    int cur_deg = DEG(g,v);
    max_deg = (max_deg<cur_deg) ? cur_deg : max_deg;
  }
  int* assocs = calloc((max_deg+1)*2, sizeof(int));
  //Scan vert's incrementing the appropriate count by it's degree
  for(int v=0; v<g->n; v++){
    int cur_deg = DEG(g,v);
    assocs[CNT_IND(cur_deg)]++;
    //Scan the vert's neighbors, incrementing the appropriate e_{ii} if they are of the same degree
    for(int n_i=g->num_edges[v]; n_i<g->num_edges[v+1]; n_i++){
      int n = g->adj[n_i];
      int n_deg = DEG(g,n);
      if(cur_deg!=0) {avg_neigh_deg[v]+=((double)n_deg)/cur_deg;}
      //Count edges only once
      if(v<n){
        if(n_deg==cur_deg){
          assocs[E_IND(cur_deg)]++;
        }
      }
    }
  }

  //Calculate degree-based assortativity coefficient
  //See Newman, "Mixing patterns in networks" Section II
  double  e_sum=0;
  double  a_sum=0;
  for(int i=0; i<max_deg; i++){
    double a_i=0;
    e_sum += assocs[E_IND(i)];
    a_i = ((double)assocs[CNT_IND(i)])*i/g->m;
    a_sum += a_i*a_i;
  }
  e_sum/=g->m;
  free(assocs);
  return (e_sum - a_sum) /(1-a_sum);
}
#undef E_IND
#undef CNT_IND
