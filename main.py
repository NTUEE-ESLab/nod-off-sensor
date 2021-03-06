from bluepy.btle import Peripheral, UUID
from bluepy.btle import Scanner, DefaultDelegate
import struct
import re
import RPi.GPIO as GPIO
import time
import RPi.GPIO as GPIO
from time import sleep


def setup():
        GPIO.setmode(GPIO.BOARD)
        GPIO.setup(16, GPIO.OUT) 
        GPIO.output(16, GPIO.HIGH)

def blink():
        print("inloop")
        GPIO.output(16, GPIO.LOW) 
        time.sleep(1)
        GPIO.output(16, GPIO.HIGH)
        time.sleep(1)

def destroy():
        GPIO.output(16, GPIO.HIGH) 
        GPIO.cleanup() # Release resource

def find_sleep():
    setup()
    GPIO.setup(7, GPIO.IN)
    while (GPIO.input(7) == False):
        print "in"
        blink()
    destroy()
    

class ScanDelegate(DefaultDelegate):
        def __init__(self):
                DefaultDelegate.__init__(self)
        def handleDiscovery(self, dev, isNewDev, isNewData):
                if isNewDev:
                        print( "Discovered device", dev.addr)
                elif isNewData:
                        print( "Received new data from", dev.addr)

scanner = Scanner().withDelegate(ScanDelegate())
devices = scanner.scan(10.0)
n=0
for dev in devices:
        print( "%d: Device %s (%s), RSSI=%d dB" % (n, dev.addr,
dev.addrType, dev.rssi))
        n += 1
        for (adtype, desc, value) in dev.getScanData():
                print( " %s = %s" % (desc, value))
number = input('Enter your device number: ')
print('Device', number)
print(devices[number].addr)
print( "Connecting...")
dev = Peripheral(devices[number].addr, 'random')
print( "Services...")
for svc in dev.services:
        print( str(svc))
try:
        setup()
        input = []
        read_or_not = True
        while(1):
                testService = dev.getServiceByUUID(UUID(0xA000))
                ledService = dev.getServiceByUUID(UUID(0xA003))
                #for ch in testService.getCharacteristics():
                #       print( str(ch))
                if read_or_not:
                       ch = dev.getCharacteristics(uuid=UUID(0xA002))[0]
                        ch1=dev.getCharacteristics(uuid=UUID(0xA004))[0]
                        if (ch.supportsRead()):
                                #print("len:")
                                #print(len(ch.read()))
                                #print("value:")
                                data = struct.unpack("1?",ch.read())[0]
                                print(data)
                                if(data == True):
                                    find_sleep()
                                    print("out")
                                    break
                                                 

finally:
        dev.disconnect()




