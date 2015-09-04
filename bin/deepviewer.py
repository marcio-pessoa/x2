import pygame, serial, time

ser = serial.Serial('/dev/serial/by-id/usb-RLRduino__www.rlrobotics.ind.br__06f8_741363339323512171F0-if00', 57600)

BLACK    = (   0,   0,   0)
RED      = ( 255,   0,   0)

RADIUS = 10

lastcmd = ""

def draw_ball(screen, x, y):
    pygame.draw.ellipse(screen, RED, [1 + x - RADIUS, y - RADIUS, RADIUS * 2, RADIUS * 2], 0)

pygame.init()

size = [800, 400]
screen = pygame.display.set_mode(size)

pygame.display.set_caption("x2-M1")

done = False

clock = pygame.time.Clock()

pygame.mouse.set_visible(0)

time.sleep(3)
ser.write("power on\r")
ser.write("gage\r")
ser.write("laser\r")

while not done:
    for event in pygame.event.get(): # User did something
        if event.type == pygame.QUIT: # If user clicked close
            done = True # Flag that we are done so we exit this loop
    
    pos = pygame.mouse.get_pos()
    x = pos[0]
    y = pos[1]
    
    device_x = -1 * (x - 400)
    device_y = y - 150
    cmd = "g " + str(device_x) + " " + str(device_y) + "\r"
    if cmd != lastcmd:
        lastcmd = cmd
        print cmd
        ser.write(cmd)
    
    if pygame.mouse.get_pressed()[0]:
        print "power\n"
        ser.write("power\r")
        
    screen.fill(BLACK)
    draw_ball(screen, x, y)  
    
    pygame.display.flip()
    
    clock.tick(40)
pygame.quit()
