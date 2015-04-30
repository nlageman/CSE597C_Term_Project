#ifndef COMMUNITY_METRICS_H
#define COMMUNITY_METRICS_H
typedef struct {
  int size;
  int intra;
  int inter;
} c_density;

double modularity(const graph_t* g, const int* coms);
double min_intra_density(const graph_t* g, const c_density* densities);
double avg_conductance (const graph_t* g, const c_density* densities);
double coverage(const graph_t* g, const c_density* densities);

//Helper that calculates the number of intra and inter
//cluster edges per community
c_density* calc_edge_densities(const graph_t* g, const int* coms);
#endif
