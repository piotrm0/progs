from col import col
from g1search import g1search

start = col(17,17,4)
#start.randomize()
rs = g1search(start,1000)
print rs.start
temp = rs.run()
print temp
