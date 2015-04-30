#include <stdlib.h>
#include <assert.h>

#include "graph.h"
#include "community_metrics.h"

double modularity(const graph_t* g, const int* coms)
{
  double q=0;
  int* ttl_degs = calloc(g->n, sizeof(int));

  // k_i = degree of vertex i, A_{ij} = # of edges between i and j; m is number of edges
  // SUM(A_{ij} - k_i * k_j/2m) = SUM(A_{ij}) - SUM(k_i * SUM(k_j/2m)), where i,j are in the same community
  // SUM(k_j) = [total degree of community of vertex  i] - k_i
  // So we store the total degrees of all the communities here => O(n) time
  for(int v=0; v<g->n; v++){
    ttl_degs[coms[v]] += g->num_edges[v+1]-g->num_edges[v];
  }

  //Here we calculate k_i * SUM(k_j/2m) for all vertices i and subtracting  each to their respective communities
  //In effect calculating the -SUM(k_i * SUM(k_j/2m))
  //O(n) time
  for(int v=0; v<g->n; v++){
    int deg = g->num_edges[v+1] - g->num_edges[v];
    q -= deg*((ttl_degs[coms[v]]-deg)/(2.0*g->num_edges[g->n]));
  }

  //Add in 1 for each edge between two vertices in the same community
  //O(m)
  for(int s=0; s<g->n; s++){
    for(int t_i=g->num_edges[s]; t_i<g->num_edges[s+1]; t_i++){
      int t=g->adj[t_i];
      q += (coms[s]==coms[t]) ? 1 : 0;
    }
  }
  q/=g->num_edges[g->n];

  free(ttl_degs);
  return q;
  }

//Helper that calculates the number of intra and inter
//cluster edges per community
c_density* calc_edge_densities(const graph_t* g, const int* coms){
  int n = g->n;
  c_density* densities  = calloc(n, sizeof(c_density));
  for(int i=0; i<n; i++){
    densities[coms[i]].size++; //
    for(int j=g->num_edges[i]; j<g->num_edges[i+1]; j++){
      int vert=g->adj[j];
      (coms[i]==coms[vert]) ? densities[coms[i]].intra++ :  densities[coms[i]].inter++;
    }
  }
  for(int i=0; i<n; i++){
    assert(densities[i].intra%2==0);
    densities[i].intra/=2; //Counted each intra cluster edge twice
  }
  return densities;
}

double min_intra_density(const graph_t* g, const c_density* densities)
{
  double min = 1.0/0.0; //INFINITY
  double d;
  for(int i=0; i<g->n; i++){
    int sz = densities[i].size;
    if(sz!=0){
      d = (sz==1) ? 1 : (((double)densities[i].intra)/sz)/(sz-1);
      min = (min<d) ? min : d;
    }
  }
  return min;
}

double avg_conductance (const graph_t* g, const c_density* densities)
{
  int num_com=0;
  int g_deg = g->n;
  double total=0.0;

  //TODO: Parallel sum this
  for(int i = 0; i<g->n; i++){
    int sz=densities[i].size;
    if(sz!=0){
      int comm_deg = 2*densities[i].intra+densities[i].inter;//community degree
      int comp_deg = g_deg - comm_deg; //complement degree
      int min_deg = (comm_deg<comp_deg) ? comm_deg : comp_deg;
      if(min_deg!=0){
        total+=((double)densities[i].inter)/min_deg;
        num_com++;
      }
    }
  }
  return total/num_com;
}

double coverage(const graph_t* g, const c_density* densities)
{
  int num_intra = 0;
  int num_edges = g->num_edges[g->n];
  //TODO: Parallel sum this
  for(int i=0; i<g->n; i++){
    num_intra+=densities[i].intra;
  }
  return ((double)num_intra)/num_edges;
}

