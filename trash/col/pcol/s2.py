from col import col
from gfnsearch import gfnsearch
from iterboth import iterboth

start = col(15,15,4)
#for c in iterboth(start, depth=2):
#    print c

#start.randomize()
rs = gfnsearch(start,depth=2,num_iters=1000000)
print rs.start
temp = rs.run()
print temp
