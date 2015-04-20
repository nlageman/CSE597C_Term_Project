import os, sys, json
import pickle
import numpy as np
import datetime
import Quandl

atoke = open("../../api_key.ig", 'r').read().strip()
def run():
  # Read in the symbols
  syms = []
  with open("../../WIKI_tickers.csv", 'r') as fp:
    fp.readline()
    for line in fp:
      syms.append(line.split(',')[0].strip())
  # Request the data for each symbol and store them in a dictionary
  for sym in syms:
    with open(os.path.join('../../data', sym.split('/')[1]), 'w+') as output:
      sym_data = Quandl.get(sym + ".1",returns="numpy",authtoken=atoke)
      pickle.dump(sym_data, output)
  


if __name__ == "__main__":
  run()
