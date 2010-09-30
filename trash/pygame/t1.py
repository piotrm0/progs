import pygame

from pygame.locals import *

SCREENRECT = Rect(0,0,640,480)

def main():
    pygame.init()

    screen = pygame.display.set_mode(SCREENRECT.size)

    background = pygame.Surface(SCREENRECT.size).convert()
    background.fill((0,0,255))
    screen.blit(background, (0,0))
    pygame.display.update()

    all = pygame.sprite.RenderUpdates()

    clock = pygame.time.Clock()

    while 1:
        for event in pygame.event.get():
            if event.type == QUIT or \
               (event.type == KEYDOWN and \
                event.key == K_ESCAPE):
                return

    all.clear(screen, background)

    all.update()

    dirty = all.draw(screen)

    pygame.display.update(dirty)

    clock.tick()

if __name__ == '__main__': main()
