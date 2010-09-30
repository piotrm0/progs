import random

class col:
    def __init__(self,width,height,colors):
        self.data  = [None] * height
        self.fails = [None] * height
        self.width  = width
        self.height = height
        self.colors = colors

        self.fitness   = 9999999
        self.num_fails = 9999999
        self.num_rects = 9999999
        
        self.rects = None

        self.iter_pos = None
        
        for y in range(height):
            self.data[y] = [0] * width
            self.data[y] = [0] * width

    def comp_rects(self):
        self.rects = []

    def randomize(self):
        for y in range(self.height):
            for x in range(self.width):
                self.data[y][x] = random.randint(0,self.colors-1)

    def randomize_rows(self):
        random.shuffle(self.data)

    def is_fail(self, x1,y1,x2,y2):
        if self.data[y1][x1] == self.data[y1][x2] and self.data[y1][x1] == self.data[y2][x1] and self.data[y1][x1] == self.data[y2][x2]: return 1
        return 0

    def rect_fitness(self, x1, y1, x2, y2):
        cols = [0] * self.colors
        for coord in [(x1,y1),(x1,y2),(x2,y1),(x2,y2)]:
            (x,y) = coord
            cols[self.data[y][x]] += 1

        cols.sort()
        cols.reverse()
        if (cols[0] == 4): return 1024
        if (cols[0] == 3): return 32
        if ((cols[0] == 2) and (cols[1] == 2)): return 4
        if (cols[0] == 2): return 1
        return 0

    def comp_num_rects(self):
        ret = 0
        for w in range(2,self.width+1):
            for h in range(2,self.height+1):
                ret += (self.width + 1 - w) * (self.height + 1 - h)
        self.num_rects = ret
        return ret

    def comp_fitness(self):
        ret = 0
        for y1 in range(self.height):
            for x1 in range(self.height):
                for y2 in range(y1+1, self.height):
                    for x2 in range(x1+1, self.width):
                        ret += self.rect_fitness(x1,y1,x2,y2)
        self.fitness = ret
        return ret

    def set_and_comp_fitness(self, x1, y1, c):
        ret     = self.fitness
        old_col = self.data[y1][x1]

        for y2 in range(self.height):
            if y2 == y1: continue
            for x2 in range(self.width):
                if x2 == x1: continue

                fitness_old = self.rect_fitness(x1,y1,x2,y2)
                self.data[y1][x1] = c
                fitness_new = self.rect_fitness(x1,y1,x2,y2)

                ret += fitness_new - fitness_old

                self.data[y1][x1] = old_col

        self.data[y1][x1] = c
        self.fitness = ret
        
        return ret

    def set_and_comp_both(self, x1, y1, c):
        ret_fitness = self.fitness
        ret_fails   = self.num_fails
        
        old_col = self.data[y1][x1]

        for y2 in range(self.height):
            if y2 == y1: continue
            for x2 in range(self.width):
                if x2 == x1: continue

                fitness_old = self.rect_fitness(x1,y1,x2,y2)
                fails_old   = self.is_fail(x1,y1,x2,y2)
                self.data[y1][x1] = c
                fitness_new = self.rect_fitness(x1,y1,x2,y2)
                fails_new   = self.is_fail(x1,y1,x2,y2)

                ret_fitness += fitness_new - fitness_old
                ret_fails   += fails_new   - fails_old

                self.data[y1][x1] = old_col

        self.data[y1][x1] = c
        
        self.fitness   = ret_fitness
        self.num_fails = ret_fails

    def set_and_comp_fails(self, x1,y1,c):
        ret = self.num_fails
        old_col = self.data[y1][x1]
        for y2 in range(self.height):
            if y2 == y1: continue
            for x2 in range(self.width):
                if x2 == x1: continue
                #print "consider (%d,%d) (%d,%d)" % (x1, y1, x2,y2)

                fail_old = self.is_fail(x1,y1,x2,y2)
                self.data[y1][x1] = c
                fail_new = self.is_fail(x1,y1,x2,y2)
                if (fail_old) and (not fail_new): ret -= 1
                if (not fail_old) and (fail_new): ret += 1
                self.data[y1][x1] = old_col

        self.data[y1][x1] = c
        self.num_fails = ret
        return ret

    def comp_num_fails(self):
        ret = 0
        for y1 in range(self.height):
            for x1 in range(self.height):
                for y2 in range(y1+1, self.height):
                    for x2 in range(x1+1, self.width):
                        if self.is_fail(x1,y1,x2,y2): ret += 1
        self.num_fails = ret
        return ret

    def copy(self):
        ret = col(self.width, self.height, self.colors)
        for y in range(self.height):
            for x in range(self.width):
                ret.data[y][x] = self.data[y][x]
        ret.num_fails = self.num_fails
        ret.num_rects = self.num_rects
        ret.fitness   = self.fitness
        return ret

    def fitness_stat_str(self):
        ret = "%d" % (self.fitness)
        return ret

    def fail_stat_str(self):
        ret = "%d / %d (%0.3f %%)" % (self.num_fails, self.num_rects, 100.0 * float(self.num_fails) / float(self.num_rects))
        return ret

    def both_stat_str(self):
        ret = "fitness: %d, fails: %d / %d (%0.3f %%)" % (self.fitness, self.num_fails, self.num_rects, 100.0 * float(self.num_fails) / float(self.num_rects))
        return ret

    def steps_both(self):
        ret = []
        for y in range(self.height):
            for x in range(self.width):
                for c in range(self.colors):
                    ret.append((x,y,c))
        return ret
    
    def __str__(self):
        ret = ""
        ret += "%d x %d (%d colors); %s\n" % (self.width, self.height, self.colors, self.both_stat_str())
        for row in self.data:
            ret += " ".join(map(str,row)) + "\n"
        return ret

    def __iter__(self):
        self.iter_pos = [0,0,-1]
        return self

    def next(self):
        self.iter_pos[2] += 1
        if self.iter_pos[2] >= self.colors:
            self.iter_pos[2] = 0
            self.iter_pos[0] += 1
            if self.iter_pos[0] >= self.width:
                self.iter_pos[0] = 0
                self.iter_pos[1] += 1
                if self.iter_pos[1] >= self.height:
                    raise StopIteration

        return self.iter_pos
                    
