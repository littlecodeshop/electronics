import RPi.GPIO as GPIO
import time
GPIO.setmode(GPIO.BCM)
GPIO.setup(17,GPIO.OUT)
p = GPIO.PWM(17,50)
p.start(7.5)      
try:             
    while True: 
        p.ChangeDutyCycle(4.5)
        time.sleep(0.5)
        p.ChangeDutyCycle(10.5)
        time.sleep(0.5)
        p.ChangeDutyCycle(7.5)
        time.sleep(0.5)
except KeyboardInterrupt:
    p.stop()
    GPIO.cleanup()
