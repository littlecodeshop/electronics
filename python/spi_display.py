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
    print("sending",byte)
    #send a 0 and then shift the whole byte
    GPIO.output(SPIDIO,False)
    time.sleep(0.0000005)
    GPIO.output(SPICLK,True)
    time.sleep(0.0000005)
    GPIO.output(SPICLK,False)
    for i in range(8):
        if((byte>>(7-i))&0b1):
            GPIO.output(SPIDIO,True)
        else:
            GPIO.output(SPIDIO,False)
        GPIO.output(SPICLK,True)
        time.sleep(0.0000005)
        GPIO.output(SPICLK,False)

#send data
def send_data(byte):
    print("sending",byte)
    #send a 1 and then shift the whole byte
    GPIO.output(SPIDIO,True)
    time.sleep(0.0000005)
    GPIO.output(SPICLK,True)
    time.sleep(0.0000005)
    GPIO.output(SPICLK,False)
    for i in range(8):
        if((byte>>(7-i))&0b1):
            GPIO.output(SPIDIO,True)
        else:
            GPIO.output(SPIDIO,False)
        GPIO.output(SPICLK,True)
        time.sleep(0.0000005)
        GPIO.output(SPICLK,False)

def reset_lcd():
    GPIO.output(SPIRES,False)
    time.sleep(0.2)
    GPIO.output(SPIRES,True)
    time.sleep(0.2)



reset_lcd()
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



send_command(0x75)
send_data(0x2)
send_data(0x20)
send_command(0x15)
send_data(0x7)
send_data(0x70)

send_command(0x5C)
for i in range(1000):
    send_data(0x0F)
    send_data(0x0F)
    send_data(0x0F)
    send_data(0x0F)

    
GPIO.cleanup()
