
from collections import namedtuple

class Pin(object):
    def __init__(self, name, index, gpio = None):
        self.name = name
        self.index = int(index)
        self.gpio = gpio

    def with_name(self, name):
        return Pin(name, self.index, self.gpio)

    def with_gpio(self, gpio):
        return self if self.gpio else Pin(self.name, self.index, gpio)

    def __str__(self):
        return '<Pin name:{} index:{} gpio:{}>'.format(self.name,self.index,self.gpio)


class PinSet(object):
    def __init__(self, pins = None, name = None, gpio = None):
        self._pins = pins or []
        self._name = name
        self._gpio = gpio

    def set_gpio(self, gpio):
        self._gpio = gpio
        self._pins = [p.with_gpio(gpio) for p in self._pins]

    def set_name(self, name):
        self._name = name
        self._pins = [p.with_name(self._name + '_' + p.name) for p in self._pins]

    def add(self, pin):
        if not pin.gpio:
            pin = pin.with_gpio(self._gpio)
        if self._name:
            pin = pin.with_name(self._name + '_' + pin.name)
        self._pins.append(pin)

    def __iter__(self):
        return iter(self._pins)

    def __str__(self):
        return '<PinSet name:{} gpio:{} pins:{}>'.format(self._name, self._gpio, ', '.join(x.name for x in self._pins))

class GpioBank(object):
    def __init__(self):
        self._banks = {}

    def add_pin(self, pin):
        assert(pin.gpio)
        gpio = pin.gpio

        bank = self._banks.get(gpio, [])
        bank.append(pin)
        self._banks[gpio] = bank

    def add_set(self, pinset):
        for p in pinset:
            self.add_pin(p)

    def dump(self):
        for bank in sorted(self._banks):
            print(bank)
            for pin in sorted(self._banks[bank], key=lambda p: p.index):
                print(pin)

    def visit_pins(self, visitor):
        for bank in sorted(self._banks):
            for pin in sorted(self._banks[bank], key=lambda p: p.index):
                visitor(pin)

    
Bus = namedtuple('Bus', ['name', 'kind', 'gpio', 'pins'])
Peripheral = namedtuple('Peripheral', ['name', 'on_bus', 'pins'])
Timer = namedtuple('Timer', ['name', 'index', 'channel', 'af'])
