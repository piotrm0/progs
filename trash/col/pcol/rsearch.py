from col import col
from search import search

class rsearch (search):
    def __init__(self, start, num_iters = 100):
        search.__init__(self, start)
        self.num_iters = num_iters
        self.num_iters_taken = 0
        
    def run(self):
        temp = self.start
        temp.randomize()
        best_fails = temp.comp_num_fails()
        best = temp.copy()
        for i in range(self.num_iters):
            temp.randomize()
            
            if temp.comp_num_fails() < best_fails:
                best = temp.copy()
                best_fails = best.comp_num_fails()
                print "iteration %d, fails: %s" % (i, best.fail_stat_str())

            if best_fails == 0:
                self.solution = best
                self.num_iters_taken = i
                return best
            
        self.num_iters_taken = self.num_iters
        self.solution = best
        return best
            
