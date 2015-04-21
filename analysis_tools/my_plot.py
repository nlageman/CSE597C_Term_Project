#Nathaniel Lageman
#CSE 597C
import numpy as np
import matplotlib.pyplot as plt

def plot_deg_dist(x_list):
  x = np.array(x_list)
  fig = plt.figure()
  plt.hist(x)
  fig.suptitle('Degree Distribution')
  plt.xlabel('Degree (d)')
  plt.ylabel('Frequency')
  plt.yscale('log', nonposy='clip')
  plt.show()

def plot_assort(x_list, avg_list):
  x = np.array(x_list)
  y = np.array(avg_list)
  fig = plt.figure()
  plt.plot(x, y, 'o')
  fig.suptitle('Assortativity')
  plt.xlabel('Degree')
  plt.ylabel('Average neighbor degree')
  #plt.ylim([0,max(y)])
  plt.yscale('log', nonposy='clip')
  plt.xscale('log', nonposy='clip')
  plt.show()

def plot_cc(cc_list):
  prob = []
  cc = []
  for i in np.arange(0, 1, 0.01):
    cc.append(i)
    prob.append(float(sum(j < i for j in cc_list))/len(cc_list))
  fig = plt.figure()
  plt.plot(prob, cc, linewidth=2)
  fig.suptitle('Cluster Coefficient')
  plt.xlabel('Cluster Coefficient (c)')
  plt.ylabel('P(x <= c)')
  plt.show()
