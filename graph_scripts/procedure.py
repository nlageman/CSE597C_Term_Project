import os, sys
import numpy as np
import pickle
import datetime
import math

# N is the number of days
# start is the first day
# load the symbol data into memory
def load_sym_data(N, start):
  data_path = '../data'
  syms = {}
  files = os.listdir(data_path)
  for f in files:
    with open(os.path.join(data_path, f), 'r') as fp:
      the_file = pickle.load(fp)
    found_start = False
    line = ''
    index = 0
    while not found_start:
      line = the_file[index]
      if line[0] == start:
        found_start = True
      index += 1
    syms[f] = []
    for i in range(index-1,index+N):
      syms[f].append(the_file[i][1])
  # syms should now hold the correct dates and the associated prices for all the
  # stock symbols 
  for sym in syms:
    print sym
    print syms[sym]
  return syms

# t is an index in the list of prices
# t = 0 represents the starting date
# t = 1 should be the starting point because of t-1
def R(sym, t):
  return math.log(syms[sym][t]/syms[sym][t-1])

# magic?
def Cij(sym_i, sym_j, N):
  RiRj = sum([R(sym_i, t)*R(sym_j,t) for t in range(1, N)])/N
  Ri_Rj = (sum([R(sym_i,t) for t in range(1, N)])/N)*(sum([R(sym_j,t) for t in range(1, N)])/N)
  top = RiRj - Ri_Rj
  vsRi = math.pow(sum([R(sym_i,t) for t in range(1, N)])/N, 2)
  vsRj = math.pow(sum([R(sym_j,t) for t in range(1, N)])/N, 2)
  bl = sum([math.pow(R(sym_i,t), 2) - vsRi for t in range(1, N)])/N
  br = sum([math.pow(R(sym_j,t), 2) - vsRj for t in range(1, N)])/N
  bottom = math.sqrt(bl*br)
  return top/bottom

if __name__ == "__main__":
  N = 10
  syms = load_sym_data(N, datetime.datetime(2015, 1, 5, 0, 0))
  theta = 0.5
  vertID = {}
  id = 0
  graph_path = '../graphs/'
  sample = 'dissim'
  with open(os.path.join(graph_path, sample+'_names.txt'), 'w+') as fp:
    for sym in syms:
      fp.write('%s\n' % sym)
      vertID[sym] = id
      id += 1
  m = 0
  with open(os.path.join(graph_path, sample+'_el.tmp'), 'w+') as fp:
    for sym_i in syms:
      for sym_j in syms:
        if sym_i != sym_j:
          #print 'Cij =', Cij(sym_i, sym_j, N), 'for i =', sym_i, 'and j =', sym_j
          if Cij(sym_i, sym_j, N) < theta:
            # add an edge
            m += 1
            fp.write('%d %d\n' % (vertID[sym_i], vertID[sym_j]))
  # the graph is now there, but we need the n and m values
  n = id
  with open(os.path.join(graph_path, sample+'_el.txt'), 'w+') as fp:
    fp.write('%s %s\n' % (n, m))
    fp2 = open(os.path.join(graph_path, sample+'_el.tmp'), 'r')
    for edge in fp2:
      fp.write(edge)
  os.system('rm '+os.path.join(graph_path, sample+'_el.tmp'))
