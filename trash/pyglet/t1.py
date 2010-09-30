import pyglet
window = pyglet.window.Window()
from pyglet.window import key
from pyglet.window import mouse

music = pyglet.resource.media('ascend.mp3')
music.play()

label = pyglet.text.Label('Hello, world',
                          font_name='Times New Roman',
                          font_size=36,
                          x=window.width//2, y = window.height//2,
                          anchor_x = 'center', anchor_y = 'center')

image = pyglet.resource.image('logo.gif')

@window.event
def on_mouse_press(x, y, button, modifiers):
    if button == mouse.LEFT:
        print "The left mouse button was pressed."

@window.event
def on_key_press(symbol, modifiers):
    print "A key was pressed"

@window.event
def on_draw():
    window.clear()
    image.blit(0,0)
    label.draw()

pyglet.app.run()
