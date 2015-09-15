#!/usr/bin/env python3

from antlr4 import *
from PinDefinitionLexer import PinDefinitionLexer
from PinDefinitionParser import PinDefinitionParser

class MyGrammarListener(ParseTreeListener):
    def enterPin_def(self, ctx):
        pass
    def exitPin_def(self, ctx):
        pass

class KeyPrinter(MyGrammarListener):     
    def exitPin_def(self, ctx):         
        pass
 

input = FileStream('../include/pins.def')
lexer = PinDefinitionLexer(input)
stream = CommonTokenStream(lexer)
parser = PinDefinitionParser(stream)
tree = parser.specification()

printer = KeyPrinter()
walker = ParseTreeWalker()
walker.walk(printer, tree)
    

