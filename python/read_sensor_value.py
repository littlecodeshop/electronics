#Capteur (light or flex or pot or distance) relié a un ADC spi et je lis dans le raspberry pi

import serial
import RPi.GPIO as GPIO
import time



SPICLK = 24
SPIMISO = 23
SPICS = 18

GPIO.setmode(GPIO.BCM)
GPIO.setup(SPICS, GPIO.OUT)
GPIO.setup(SPIMISO, GPIO.IN,pull_up_down=GPIO.PUD_UP)
GPIO.setup(SPICLK, GPIO.OUT)


#au debut
GPIO.output(SPICS,True)
GPIO.output(SPICLK,False)


while True:
    GPIO.output(SPICS,False)
    GPIO.output(SPICLK,True)
    GPIO.output(SPICLK,False)
    GPIO.output(SPICLK,True)
    #GPIO.output(SPICLK,False)
#la le null bit
    GPIO.output(SPICLK,False)
    GPIO.output(SPICLK,True)
    adcout = 0
#maintenant je lis les donnees 
    for i in range(12):
        GPIO.output(SPICLK,False)
        if GPIO.input(SPIMISO):
            adcout |=0x1
        GPIO.output(SPICLK,True)    
        if i<11:
            adcout <<= 1 #left shift 
    GPIO.output(SPICS,True)
    print("{0:b}".format(adcout)," - ",adcout)
    
GPIO.cleanup()


