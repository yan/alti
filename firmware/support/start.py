#!/usr/bin/env python3

from antlr4 import *
from pin_def import *
from PinDefinitionLexer import PinDefinitionLexer
from PinDefinitionParser import PinDefinitionParser

class GPIO(object):
    def __init__(self):
        self.gpio_banks = {}

    def add_pin(self, pin):
        assert(pin.gpio)
        if not pin.gpio in self.gpio_banks:
            self.gpio_banks[pin.gpio] = [pin]
        else:
            self.gpio_banks[pin.gpio].append(pin)

    def extend(self, rhs):
        for bank in self.gpio_banks:
            if bank in rhs.gpio_banks:
                self.gpio_banks[bank].extend(rhs.gpio_banks[bank])

    def dump(self):
        for bank in self.gpio_banks:
            print(bank)
            for p in self.gpio_banks[bank]:
                print(p)

class MyGrammarListener(ParseTreeListener):
    def __init__(self):
        super().__init__()
        self.banks = GpioBank()
    def on_pin(self, pin):
        self.banks.add_pin(pin)
        #self.banks.add_pin(pin)
    def on_bus(self, bus):
        bus.pins.set_name(bus.name)
        bus.pins.set_gpio(bus.gpio)
        self.banks.add_set(bus.pins)
        print('{}'.format(bus.pins))

    def on_peripheral(self, peripheral):
        peripheral.pins.set_name(peripheral.name)
        self.banks.add_set(peripheral.pins)

    def enterSpecification(self, ctx):
        for v in ctx.spec:
            if isinstance(v, Pin): self.on_pin(v)
            elif isinstance(v, Bus): self.on_bus(v)
            elif isinstance(v, Peripheral): self.on_peripheral(v)
 

input = FileStream('../include/pins.def')
lexer = PinDefinitionLexer(input)
stream = CommonTokenStream(lexer)
parser = PinDefinitionParser(stream)
tree = parser.specification()

listener = MyGrammarListener()
walker = ParseTreeWalker()
walker.walk(listener, tree)
print(listener.banks.dump())

