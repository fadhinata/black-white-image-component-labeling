import pygame

#----------------------------------------------------------------
# generate a file of ones and zeros based on the binary image
# for use in our component labeling algorithm
#
# This also redisplays the image as a sanity check
#
# I run this as:
#
#       python binaryimage.py > connected_test_binary.txt
#
#----------------------------------------------------------------

pygame.init()

# we assume that connected_test.png has only black and
# whitye pixels.
pic = pygame.image.load("connected_test.png")

win = pygame.display.set_mode((pic.get_width(), pic.get_height()))

for y in range(pic.get_height()):
    for x in range(pic.get_width()):
        c = pic.get_at((x,y))
        if c.r == 0:
            win.set_at((x,y), (0,0,255))
            print 1,
        elif c.r == 255:
            win.set_at((x,y), (0,0,0))
            print 0,
        else:
            print "should never get here"

    print
    pygame.display.update()