#!/usr/bin/env python
#Nathaniel Lageman
#CSE 597C
import sys, os

def read_graph(file_el):
  print 'reading graph'
  fp = open(file_el, 'r')
  n, m = fp.readline().split()
  n = int(n)
  m = int(m)
  print 'Number of vertices: %d' % n
  print 'Number of edges: %d' % m
  print 'first open for degrees...'
  with open('./DOT/'+file_el[:-4]+'.dot', 'w+') as output:
    print 'opened new dot:'+'./DOT/'+file_el[:-4]+'.dot'
    output.write('strict graph {\n')
    for line in fp:
      u, v = line.split()
      u = int(u)
      v = int(v)
      if v > u:
        output.write("\t%d -- %d\n" % (u, v))
    output.write('}')
  fp.close()

if __name__ == "__main__":
  input_file_el = sys.argv[-1]
  print 'making dot file'
  read_graph(input_file_el)
