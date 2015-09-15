
from collections import namedtuple

class Pin(namedtuple('Pin', ['name', 'index', 'gpio'])):
    def __new__(cls, name, index, gpio=None):
        return super(Pin, cls).__new__(cls, name, index, gpio)
    
Bus = namedtuple('Bus', ['name', 'kind', 'gpio', 'pins'])
Peripheral = namedtuple('Peripheral', ['name', 'on_bus', 'pins'])
Timer = namedtuple('Timer', ['name', 'index', 'channel', 'af'])

class PinSet(object):
    def __init__(self, pins):
        self.pins = pins
    def gpio(self, gpio):
        self.gpio = gpio
    def add(self, pin):
        self.pins.append(pin)

