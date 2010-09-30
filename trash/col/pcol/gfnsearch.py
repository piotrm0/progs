from col import col
from search import search
from iterboth import iterboth

class gfnsearch (search):
    def __init__(self, start, depth = 1, num_iters = 100):
        search.__init__(self, start)
        self.depth = depth
        self.num_iters = num_iters
        self.num_iters_taken = 0

    def run(self):
        temp = self.start
        temp.comp_fitness();
        temp.comp_num_fails();
        #temp.comp_num_rects();

        prev_fitness   = -1
        #prev_num_fails = -1

        for i in range(self.num_iters):
            temp.randomize_rows()
            
            print "iteration %d fitness: %s" % (i, temp.fitness_stat_str())
            print temp

            best = temp.copy()

            for steps in iterboth(temp, depth=self.depth):
                if not (steps[0][1] % 10): print "\r%s" % (steps),
                num_steps = len(steps)
                work = temp.copy()
                step_num = 0
                for step in steps:
                    step_num += 1
                    (c,x,y) = step
                    work.set_and_comp_fitness(x,y,c)

                #if work.num_fails < best.num_fails or ((work.num_fails == best.num_fails) and (work.fitness < best.fitness)):
                if work.fitness < best.fitness:
                    best = work.copy()
                    break
                
            print "\n",
                    
            temp = best
            temp.comp_num_fails()

            #if (temp.num_fails) == 0 or ((temp.num_fails == prev_num_fails) and (temp.fitness == prev_fitness)):
            if (temp.num_fails == 0) or (temp.fitness == prev_fitness):
                self.solution = temp
                self.num_iters_taken = i
                return best

            prev_fitness   = temp.fitness
            #prev_num_fails = temp.num_fails
            
        self.num_iters_taken = self.num_iters
        self.solution = temp
        
        return temp
            
