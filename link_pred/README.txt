Project Team: Jared Bean and Nathaniel Lageman

We implemented Adamic/Adar and Katz algorithm. Performance wise, Adamic/Adar
was far superior to Katz. For this reason, our implementation of Katz was
truncated to 10 iterations, i.e. up to paths of length 10. We got some
interesting results between the two of them. The top 10/100 list for
Adamic/Adar are included as adamic_10.dat and adamic_100.dat. The top 10/100
list of Katz is inclded as katz_10.dat and katz_100.dat. We ran the Katz link
prediction algorithm with a beta value of 0.05. In our test, beta = 0.05,
preformed the most sensibly.
