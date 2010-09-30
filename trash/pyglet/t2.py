import pyglet
import numpy
from pyglet.gl import *
from pyglet.window import key
from pyglet.window import mouse

from math import pi,sin,cos,log10,pow,acos,sqrt,asin
from random import uniform

window = pyglet.window.Window()
platform = pyglet.window.get_platform()
display  = platform.get_default_display()
screen   = display.get_default_screen()

#music = pyglet.resource.media('ascend.mp3')
#music.play()

#image = pyglet.resource.image('logo.gif')

pointsize = 1.0

for screen in display.get_screens():
    print screen

class lmap:
    def __init__(self, b1, b2):
        w1 = float(b1[1][0]) - float(b1[0][0])
        w2 = float(b2[1][0]) - float(b2[0][0])

        h1 = float(b1[1][1]) - float(b1[0][1])
        h2 = float(b2[1][1]) - float(b2[0][1])

        self.mx1 = float(b1[0][0])
        self.mx2 = float(b2[0][0])
        self.my1 = float(b1[0][1])
        self.my2 = float(b2[0][1])

        self.rw = w2 / w1
        self.rh = w2 / w1

    def trans_to(self):
        glTranslatef(self.mx2, self.my2, 0)
        glScalef(self.rw, self.rh, 1.0)
        glTranslatef(-self.mx1, -self.my1, 0)

    def trans_from(self):
        glTranslatef(-self.mx1, self.my1, 0)
        glScalef(1.0/self.rw, 1.0/self.rh, 1.0)
        glTranslatef(self.mx2, self.my2, 0)

    def map_to(self, p):
        return [(p[0] - self.mx1) * self.rw + self.mx2,
                (p[1] - self.my1) * self.rh + self.my2]

    def map_from(self, p):
        return [(p[0] - self.mx2) / self.rw + self.mx1,
                (p[1] - self.my2) / self.rh + self.my1]

class vector:
    def __init__(self, c=[0.0, 0.0]):
        self.c = [c[0], c[1]]

        self.norm()

    def norm(self):
        m = self.mag()

        self.c[0] /= m
        self.c[1] /= m

    def mag(self):
        return sqrt(dot(self, self))

def polygon_draw(pts):
    glColor4f(0,1,0,0.1)
    glBegin(GL_POLYGON)
    for p in pts:
        p.vertex()

    glEnd()

    glColor4f(0,1,0,1.0)
    glBegin(GL_LINE_LOOP)
    for p in pts:
        p.vertex()

    glEnd()

def ch(pts):
    pts.sort(lambda a,b: cmp(a.c[0], b.c[0]))

    stack = [[pts[0], pts[1]],
             [pts[0], pts[1]]]

    m = [1, -1]

    for i in range(2):
        for pi in pts[2:]:
            pfirst  = stack[i][-1]
            psecond = stack[i][-2]

            while ((orient(pi, pfirst, psecond) * m[i] <= 0) and (len(stack[i]) >= 2)):
                stack[i].pop()

                if (len(stack[i]) >= 2):
                    pfirst  = stack[i][-1]
                    psecond = stack[i][-2]

            stack[i].append(pi)

    stack[1].reverse()

    return stack[0] + stack[1][1:-1]
            
def angle(u,v):
    return acos(dot(u,v))

def dot(u,v):
    ret = 0
    for i in range(2):
        ret += u.c[i] * v.c[i]
    return ret

def vector_of_points(p1, p2):
    return vector([p1.c[0] - p2.c[0],
                   p1.c[1] - p2.c[1]])

def det(m):
    return (m[0][0] * m[1][1] * m[2][2] -
            m[0][0] * m[1][2] * m[2][1] +
            m[0][1] * m[1][2] * m[1][0] -
            m[0][1] * m[1][0] * m[2][2] +
            m[0][2] * m[1][0] * m[2][1] -
            m[0][2] * m[1][1] * m[2][0])

def orient(p, q, r):
    m = numpy.array([[1.0, p.c[0], p.c[1]],
                     [1.0, q.c[0], q.c[1]],
                     [1.0, r.c[0], r.c[1]]])

    temp =  numpy.linalg.det(m)

    return temp

def points_angle(p, q, r):
    u = vector_of_points(p, q)
    v = vector_of_points(r, q)

    #return angle(u, v)

    temp = orient(q,p,r) / (u.mag() * v.mag())

    return asin(temp)

def draw_line(p1, p2):
    glLineWidth(5)
    
    glColor3f(0.0,1.0,0.0)
    
    glBegin(GL_LINES)
    p1.vertex()
    p2.vertex()
    glEnd()

    glLineWidth(1)
    
    glColor3f(0.0,0.5,0.0)
    
    glBegin(GL_LINES)
    p1.vertex()
    p2.vertex()
    glEnd()

class point:
    def __init__(self,
                 c = [0.0, 0.0],
                 v = [0.0, 0.0],
                 grav = -0.0,
                 range = [[-1.0, -1.0], [1.0,1.0]]):
        self.c = c
        self.v = v
        self.grav = grav
        self.range = range

    def vertex(self):
        glVertex2f(self.c[0], self.c[1])

    def random_pos(self):
        self.c = [uniform(self.range[0][0],
                          self.range[1][0]),
                  uniform(self.range[0][1],
                          self.range[1][1])]

    def random_vel(self):
        self.v = [uniform(self.range[0][0],
                          self.range[1][0]) / 10.0,
                  uniform(self.range[0][1],
                          self.range[1][1]) / 10.0]

    def mark(self, dt):
        self.c[0] += self.v[0] * dt
        self.c[1] += self.v[1] * dt
        self.v[1] += self.grav * dt

        for i in range(2):
            if self.c[i] < self.range[0][i]:
                self.c[i] = self.range[0][i]
                self.v[i] *= -1.0
            elif self.c[i] > self.range[1][i]:
                self.c[i] = self.range[1][i]
                self.v[i] *= -1.0

    def draw(self):
        global pointsize
        
        glBegin(GL_POLYGON)
        glColor3f(0.0,0.5,0.0)
        circle_vertices(self.c, pointsize)
        glEnd()

        glLineWidth(2)
        glBegin(GL_LINE_LOOP)
        glColor3f(0.0,1.0,0.0)
        circle_vertices(self.c, pointsize)
        glEnd()

def draw_angle(p, r, a1, a2):
    da = (a2 - a1) / 20.0

    glColor4f(0,1,0, 0.5)

    glBegin(GL_POLYGON)
    glVertex2f(p.c[0], p.c[1])

    a = a1
    for i in range(21):
        glVertex2f(p.c[0] + r * cos(a),
                   p.c[1] + r * sin(a))
        a += da

    glEnd()

class space:
    def __init__(self):
        global pointsize
        
        self.points = []
        self.range = [[-1.0,-1.0],[1.0,1.0]]
        self.range_actual = self.compute_range_with_aspect(float(window.width) / float(window.height))
        pointsize = self.compute_point_size()

        self.lmap = lmap(self.range_actual, [[0,0],[window.width, window.height]])

    def compute_range_with_aspect(self, a, padding = 1.1):
        w = float(self.range[1][0] - self.range[0][0]) * padding
        h = float(self.range[1][1] - self.range[0][1]) * padding

        wc = float(self.range[1][0] + self.range[0][0]) / 2.0
        hc = float(self.range[1][1] + self.range[0][1]) / 2.0

        if a >= 1.0:
            w = w * a
        else:
            h = h / a

        return [[wc - w / 2.0, hc - h / 2.0],
                [wc + w / 2.0, hc + h / 2.0]] 

    def compute_point_size(self):
        return (float(self.range[1][0]-self.range[0][0]) / 100.0)

    def setup_view(self):
        #glMatrixMode(GL_PROJECTION)
        #glLoadIdentity()
        #glOrtho(self.range_actual[0][0], self.range_actual[1][0],
        #        self.range_actual[0][1], self.range_actual[1][1],
        #        0.0, 2)

        glMatrixMode(GL_MODELVIEW)
        glLoadIdentity()
        self.lmap.trans_to()
        
    def add_random_points(self, num=10):
        for i in range(num):
            p = point(range = self.range)
            p.random_pos()
            p.random_vel()
            
            self.points.append(p)

    def draw_bg(self):
        glColor3f(0.0, 0.05, 0.0)
        glBegin(GL_QUADS)
        glVertex2f(self.range_actual[0][0], self.range_actual[0][1])
        glVertex2f(self.range_actual[1][0], self.range_actual[0][1])
        glVertex2f(self.range_actual[1][0], self.range_actual[1][1])
        glVertex2f(self.range_actual[0][0], self.range_actual[1][1])
        glEnd()

        glColor3f(0.0, 0.065, 0.0)
        glBegin(GL_QUADS)
        glVertex2f(self.range[0][0], self.range[0][1])
        glVertex2f(self.range[1][0], self.range[0][1])
        glVertex2f(self.range[1][0], self.range[1][1])
        glVertex2f(self.range[0][0], self.range[1][1])
        glEnd()

        w = self.range_actual[1][0] - self.range_actual[0][0]
        h = self.range_actual[1][1] - self.range_actual[0][1]

        maxsize = max(w,h)

        wc = (self.range_actual[0][0] + self.range_actual[1][0]) / 2.0
        hc = (self.range_actual[0][1] + self.range_actual[1][1]) / 2.0

        dx = [pow(10, round(log10(float(maxsize) / 10.0))),
              pow(10, round(log10(float(maxsize) / 5.0)))]
        count = [float(int(maxsize / dx[0])) + 1,
                 float(int(maxsize / dx[1])) + 1]

        glLineWidth(1)

        glDisable(GL_LINE_SMOOTH)

        glBegin(GL_LINES)

        for j in range(2):
            if j == 0:
                glColor3f(0.0, 0.1, 0.0)
            else:
                glColor3f(0.0, 0.5, 0.0)
            
            if (int(count[j]) % 2 != 0): count[j] += 1

            x = wc - (count[j] / 2.0) * dx[j]
            for i in range(int(count[j])):
                glVertex2f(x, self.range_actual[0][1])
                glVertex2f(x, self.range_actual[1][1])
                x += dx[j]

            x = hc - (count[j] / 2.0) * dx[j]
            for i in range(int(count[j])):
                glVertex2f(self.range_actual[0][0], x)
                glVertex2f(self.range_actual[1][0], x)
                x += dx[j]

        glEnd()

        glEnable(GL_LINE_SMOOTH)
        
    def mark(self, dt):
        for p in self.points:
            p.mark(dt)

    def draw_points(self):
        for p in self.points:
            p.draw()

    def draw_space(self):
        self.draw_bg()

        p = ch(self.points)

        polygon_draw(p)

        self.draw_points()

        #p1 = self.points[0]
        #p2 = self.points[1]
        #p3 = self.points[2]

        #ph = point(c = [p2.c[0] + 1, p2.c[1]])

        #o = orient(p1, p2, p3)

        #draw_line(p1, p2)
        #draw_line(p2, p3)

        #glMatrixMode(GL_PROJECTION)
        #ah = points_angle(ph, p2, p1)
        #a = points_angle(p1, p2, p3)
        #draw_angle(p2, 0.5, ah, pi)
        
        #glPushMatrix()
        #glLoadIdentity()

        #glOrtho(0, window.width, 0, window.height, 0, 1)
        #glMatrixMode(GL_MODELVIEW)

        #temp = self.lmap.map_to(p2.c)

        #glTranslatef(temp[0],temp[1], 0)
        #glRotatef(a * 180.0 / pi, 0.0, 0.0, 1.0)

        #label = pyglet.text.Label(str((a, ah, a+ah)),
        #                          font_name='Lucida Console',
        #                          font_size=10,
        #                          x = 0, y = 0,
        #                          anchor_x = 'left', anchor_y = 'center')

        #label.draw()

        #glMatrixMode(GL_PROJECTION)
        
        #glPopMatrix()
        
        #glMatrixMode(GL_MODELVIEW)

def circle_vertices(p, r=1.0, pieces=10):
    a = 0.0
    da = pi * 2.0 / float(pieces)
    for i in range(pieces):
        glVertex2f(p[0] - r * sin(a),
                   p[1] - r * cos(a))
        a += da

@window.event
def on_mouse_press(x, y, button, modifiers):
    if button == mouse.LEFT:
        print "The left mouse button was pressed."

@window.event
def on_key_press(symbol, modifiers):
    print "A key was pressed"

s = space()
s.add_random_points(10)

window.double_buffer = True
glEnable(GL_LINE_SMOOTH);
glEnable(GL_BLEND);
glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);

fps_display = pyglet.clock.ClockDisplay(clock = pyglet.clock.schedule_interval(s.mark, 1.0/50.0))

glMatrixMode(GL_PROJECTION)
glLoadIdentity()
glOrtho(0, window.width, 0, window.height, 0, 1)
glMatrixMode(GL_MODELVIEW)

@window.event
def on_draw():
    glClear(GL_COLOR_BUFFER_BIT)

    glMatrixMode(GL_MODELVIEW)
    glLoadIdentity()

    glPushMatrix()
    s.setup_view()
    s.draw_space()
    glPopMatrix()
    
    fps_display.draw()

#    draw_point((10,10))
#    glBegin(GL_TRIANGLES)
#    glVertex2f(0, 0)
#    glVertex2f(window.width, 0)
#    glVertex2f(window.width, window.height)
#    glEnd()
    
    #window.clear()
    #image.blit(0,0)
    #label.draw()
    
pyglet.app.run()
