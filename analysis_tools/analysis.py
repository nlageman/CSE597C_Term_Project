#!/usr/bin/env python
#Nathaniel Lageman
#CSE 597C
import sys, os
import my_plot

# design of g
#  n;          /* number of vertices */
#  m;          /* number of edges */
#  adj;        /* adj[num_edges[i]..num_edges[i+1]-1] gives 
#                 vertices adjacent to vertex i */
#  num_edges;  /* num_edges[i+1]-num_edges[i] gives degree of 
#                 vertex i */ 
def analyze(g):
  n = g['n']
  m = g['m']
  print 'finding degree distribution and assortitivity'
  # find degree distribution
  deg = []
  avg_deg = []
  for i in range(n):
    degree_i = g['num_edges'][i+1] - g['num_edges'][i]
    deg.append(degree_i)
    temp_degs = []
    for j in range(g['num_edges'][i], g['num_edges'][i+1]-1):
      u = g['adj'][j]
      degree_u = g['num_edges'][u+1] - g['num_edges'][u]
      temp_degs.append(degree_u) 
    if len(temp_degs) > 0:
      avg_deg.append(sum(temp_degs)/len(temp_degs))
    else:
      avg_deg.append(0)
      
  my_plot.plot_deg_dist(deg)
  my_plot.plot_assort(deg, avg_deg) 
  
  print 'finding clustering coefficient distribution'
  cluster_co = []
  for i in range(n):
    k_i = g['num_edges'][i+1] - g['num_edges'][i]
    t_g = (k_i*(k_i - 1))/2
    num_tris = 0.0
    temp_adj = {}
    for j in range(g['num_edges'][i], g['num_edges'][i+1]-1):
      temp_adj[g['adj'][j]] = True # use dict for const time check in N(i)
    for vert in temp_adj:
      for k in range(g['num_edges'][vert], g['num_edges'][vert+1]-1):
        u = g['adj'][k]
        if u in temp_adj:
          if temp_adj[u]:
            num_tris += 1
      temp_adj[vert] = False
    if t_g == 0:
      cluster_co.append(0)
    else:
      cc = num_tris/t_g
      cluster_co.append(cc)
  #print cluster_co
  print 'plotting cluster coefficient'
  my_plot.plot_cc(cluster_co)
  


  return 0

#essentially the python version of the read_graph function from centralities.c
def read_graph(file_el, file_names):

  print 'reading graph'
  fp = open(file_el, 'r')
  n, m = fp.readline().split()
  n = int(n)
  m = int(m)
  print 'Number of vertices: %d' % n
  print 'Number of edges: %d' % m
  print 'first open for degrees...'
  degrees = []
  for i in range(n):
    degrees.append(0)
  for line in fp:
    u, v = line.split()
    u = int(u)
    v = int(v)
    degrees[u] += 1
    degrees[v] += 1
  fp.close()
  
  num_edges = []
  adj = []
  num_edges.append(0)
  for i in range(1, n+1):
    num_edges.append(num_edges[i-1] + degrees[i-1])

  print 'second open for adjacency list'
  fp = open(file_el, 'r')
  fp.readline()
  for i in range(n):
    degrees[i] = 0
  for i in range(2*m):
    adj.append(0)
  for line in fp:
    u = int(line.split()[0])
    v = int(line.split()[1])
    uv_loc = num_edges[u] + degrees[u]
    vu_loc = num_edges[v] + degrees[v]
    adj[uv_loc] = v
    adj[vu_loc] = u
    degrees[u] += 1
    degrees[v] += 1

  g = {}
  g['n'] = n
  g['m'] = m
  g['num_edges'] = num_edges
  g['adj'] = adj

  print 'finished reading in graph'
  return g


if __name__ == "__main__":
  input_file_el = sys.argv[-2]
  input_file_names = sys.argv[-1]
  g = read_graph(input_file_el, input_file_names)
  analyze(g)
