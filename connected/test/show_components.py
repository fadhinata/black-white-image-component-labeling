import pygame
import random

#-----------------------------------------------------
# author: Ed Harcourt
#
# Date: 1/11/12
#
# Given a labeled image file generate a colored map
# of the file to make sure the labeling worked.
#
#  The input file is structured something like
#
#    0 0 0 0 0 0
#    0 1 0 1 0 0
#    0 1 1 1 0 0
#    0 0 0 0 0 0
#    0 2 2 2 2 0
#    0 0 0 0 0 0
#    0 0 3 3 0 0
#
#------------------------------------------------------

# initialize pygame and construct a window
# for displaying the image
pygame.init()
win = pygame.display.set_mode((640, 480))
white = (255,255,255)
win.fill(white)

y = 0 # rows

# We map a label number to a color
color_map = {}
done = False

while not done:
    for line in file("connected_test_labeled.txt"):
        row = line.split()
        x = 0
        for val in row:
            x = x + 1
            ival = int(val)
            if ival == 0: continue  # skip background pixels
            
            if color_map.get(ival, 0) == 0:
                r = random.randrange(255)
                g = random.randrange(255)
                b = random.randrange(255)
                color_map[ival] = (r,g,b)
                
            win.set_at((x,y), (color_map[ival]))
            
        pygame.display.update()
        y = y + 1
        
    
    # wait for the user to click on quit
    for event in pygame.event.get():
        
        # check for a click on the upper left x
        if event.type == pygame.QUIT:
            done = True
            
            
print "Saveing labeled image as connected_test_labeled.png"
pygame.image.save(win, "connected_test_labeled.png")