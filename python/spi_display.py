# NOKIA 6100 SPI display

import RPi.GPIO as GPIO
import time



SPICLK  = 21
SPIDIO  = 24
SPICS   = 18
SPIRES  = 23

GPIO.setmode(GPIO.BCM)
GPIO.setup(SPICS, GPIO.OUT)
GPIO.setup(SPIDIO, GPIO.OUT)
GPIO.setup(SPICLK, GPIO.OUT)
GPIO.setup(SPIRES, GPIO.OUT)

GPIO.output(SPICLK,False)
GPIO.output(SPICS,False)

#send command
def send_command(byte):
    #print("sending",byte)
    #send a 0 and then shift the whole byte
    GPIO.output(SPIDIO,False)
    #time.sleep(0.000000015)
    GPIO.output(SPICLK,True)
    #time.sleep(0.000000015)
    GPIO.output(SPICLK,False)
    for i in range(8):
        if((byte>>(7-i))&0b1):
            GPIO.output(SPIDIO,True)
        else:
            GPIO.output(SPIDIO,False)
        GPIO.output(SPICLK,True)
        #time.sleep(0.000000015)
        GPIO.output(SPICLK,False)

#send data
def send_data(byte):
    #print("sending",byte)
    #send a 1 and then shift the whole byte
    GPIO.output(SPIDIO,True)
    #time.sleep(0.000000015)
    GPIO.output(SPICLK,True)
    #time.sleep(0.000000015)
    GPIO.output(SPICLK,False)
    for i in range(8):
        if((byte>>(7-i))&0b1):
            GPIO.output(SPIDIO,True)
        else:
            GPIO.output(SPIDIO,False)
        GPIO.output(SPICLK,True)
        #time.sleep(0.000000015)
        GPIO.output(SPICLK,False)

def reset_lcd():
    GPIO.output(SPIRES,False)
    time.sleep(0.2)
    GPIO.output(SPIRES,True)
    time.sleep(0.2)
    send_command(0xCA)
    send_data(0x00)
    send_data(0x20)
    send_data(0x00)
    send_command(0xBB)
    send_data(0x01)
    send_command(0xD1)
    send_command(0x94)
    send_command(0x20)
    send_data(0x0f)
    send_command(0xA7)
    send_command(0xBC)
    send_data(0x01)
    send_data(0x00)
    send_data(0x02)
    send_command(0x81)
    send_data(32)
    send_data(3)
    time.sleep(0.5)
    send_command(0xAF)

def fill_rect(x1,x2,y1,y2,r,g,b):
    send_command(0x75)
    send_data(x1)
    send_data(x2)
    send_command(0x15)
    send_data(y1)
    send_data(y2)
    l = (x2-x1)*(y2-y1)
    send_command(0x5C)
#en 3 bytes -> 2 points 
    byte1 = r<<4|g
    byte2 = b<<4|r
    byte3 = g<<4|b
    for i in range(l/2):
        send_data(byte1)
        send_data(byte2)
        send_data(byte3)

def clear_screen(r,g,b):
    fill_rect(0,132,0,132,r,g,b)


#main
reset_lcd()
clear_screen(0,0,0)
fill_rect(0,40,0,40,0x0,0x0,0xF)
fill_rect(40,80,40,80,0x0,0xF,0x0)
fill_rect(80,120,80,120,0xF,0x0,0x0)



while True:
    pass
    
GPIO.cleanup()
