from col import col
from search import search

class g1search (search):
    def __init__(self, start, num_iters = 100):
        search.__init__(self, start)
        self.num_iters = num_iters
        self.num_iters_taken = 0

    def run(self):
        temp = self.start
        temp.comp_num_fails();

        prev_num_fails = -1

        for i in range(self.num_iters):
            print "iteration %d fails: %s" % (i, temp.fail_stat_str())
            print temp
            #opts = []

            best = temp.copy()

            #for step in temp.steps_both():
            for step in temp:
                (x,y,c) = step
                
                prev_col = temp.data[y][x]
                prev_fails = temp.num_fails;
                
                #temp.data[y][x] = c
                temp.set_and_comp_fails(x,y,c)
                #temp.num_fails = None
                #temp.comp_num_fails()
                if temp.num_fails < best.num_fails:
                    best = temp.copy()
                    temp.data[y][x] = prev_col
                    temp.num_fails  = prev_fails
                    #break

                temp.data[y][x] = prev_col
                temp.num_fails  = prev_fails

            temp = best

            if temp.num_fails == 0 or temp.num_fails == prev_num_fails:
                self.solution = temp
                self.num_iters_taken = i
                return best

            prev_num_fails = temp.num_fails
            
        self.num_iters_taken = self.num_iters
        self.solution = temp
        
        return temp
            
