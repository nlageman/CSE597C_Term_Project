import os, sys
import numpy as np
import pickle
import datetime

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
      syms[f].append(the_file[i])
  # syms should now hold the correct dates and the associated prices for all the
  # stock symbols 
  for sym in syms:
    print sym
    print syms[sym]

if __name__ == "__main__":
  load_sym_data(10, datetime.datetime(2015, 1, 5, 0, 0))
