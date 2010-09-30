class iterboth:
    def __init__(self, col, depth):
        self.col   = col
        self.depth = depth

        self.iter_pos = [None] * depth
        self.pos_max  = [None] * depth

    def __iter__(self):
        for d in range(self.depth):
            self.iter_pos[d] = [0,0,0]
            self.pos_max[d] = [self.col.colors-1, self.col.width-1, self.col.height-1]
            
        self.iter_pos[0][0] = -1
        return self

    def next(self):
        dpos = 0
        ppos = 0
        self.iter_pos[dpos][ppos] += 1
        
        while (self.iter_pos[dpos][ppos] > self.pos_max[dpos][ppos]):
            self.iter_pos[dpos][ppos] = 0
            ppos += 1
            if ppos >= 3:
                dpos += 1
                ppos = 0
                if dpos >= self.depth:
                    raise StopIteration
            self.iter_pos[dpos][ppos] += 1

        return self.iter_pos
