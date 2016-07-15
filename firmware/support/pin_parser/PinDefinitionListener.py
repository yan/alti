# Generated from ./PinDefinition.g4 by ANTLR 4.5.1
from antlr4 import *
if __name__ is not None and "." in __name__:
    from .PinDefinitionParser import PinDefinitionParser
else:
    from PinDefinitionParser import PinDefinitionParser

from pin_def import *
#el = []


# This class defines a complete listener for a parse tree produced by PinDefinitionParser.
class PinDefinitionListener(ParseTreeListener):

    # Enter a parse tree produced by PinDefinitionParser#specification.
    def enterSpecification(self, ctx:PinDefinitionParser.SpecificationContext):
        pass

    # Exit a parse tree produced by PinDefinitionParser#specification.
    def exitSpecification(self, ctx:PinDefinitionParser.SpecificationContext):
        pass


    # Enter a parse tree produced by PinDefinitionParser#definition.
    def enterDefinition(self, ctx:PinDefinitionParser.DefinitionContext):
        pass

    # Exit a parse tree produced by PinDefinitionParser#definition.
    def exitDefinition(self, ctx:PinDefinitionParser.DefinitionContext):
        pass


    # Enter a parse tree produced by PinDefinitionParser#bus_def.
    def enterBus_def(self, ctx:PinDefinitionParser.Bus_defContext):
        pass

    # Exit a parse tree produced by PinDefinitionParser#bus_def.
    def exitBus_def(self, ctx:PinDefinitionParser.Bus_defContext):
        pass


    # Enter a parse tree produced by PinDefinitionParser#pins_def.
    def enterPins_def(self, ctx:PinDefinitionParser.Pins_defContext):
        pass

    # Exit a parse tree produced by PinDefinitionParser#pins_def.
    def exitPins_def(self, ctx:PinDefinitionParser.Pins_defContext):
        pass


    # Enter a parse tree produced by PinDefinitionParser#pin_list.
    def enterPin_list(self, ctx:PinDefinitionParser.Pin_listContext):
        pass

    # Exit a parse tree produced by PinDefinitionParser#pin_list.
    def exitPin_list(self, ctx:PinDefinitionParser.Pin_listContext):
        pass


    # Enter a parse tree produced by PinDefinitionParser#single_pin_def.
    def enterSingle_pin_def(self, ctx:PinDefinitionParser.Single_pin_defContext):
        pass

    # Exit a parse tree produced by PinDefinitionParser#single_pin_def.
    def exitSingle_pin_def(self, ctx:PinDefinitionParser.Single_pin_defContext):
        pass


    # Enter a parse tree produced by PinDefinitionParser#pin_def.
    def enterPin_def(self, ctx:PinDefinitionParser.Pin_defContext):
        pass

    # Exit a parse tree produced by PinDefinitionParser#pin_def.
    def exitPin_def(self, ctx:PinDefinitionParser.Pin_defContext):
        pass


    # Enter a parse tree produced by PinDefinitionParser#timer_def.
    def enterTimer_def(self, ctx:PinDefinitionParser.Timer_defContext):
        pass

    # Exit a parse tree produced by PinDefinitionParser#timer_def.
    def exitTimer_def(self, ctx:PinDefinitionParser.Timer_defContext):
        pass


    # Enter a parse tree produced by PinDefinitionParser#def_tuple.
    def enterDef_tuple(self, ctx:PinDefinitionParser.Def_tupleContext):
        pass

    # Exit a parse tree produced by PinDefinitionParser#def_tuple.
    def exitDef_tuple(self, ctx:PinDefinitionParser.Def_tupleContext):
        pass


    # Enter a parse tree produced by PinDefinitionParser#def_tuple_list.
    def enterDef_tuple_list(self, ctx:PinDefinitionParser.Def_tuple_listContext):
        pass

    # Exit a parse tree produced by PinDefinitionParser#def_tuple_list.
    def exitDef_tuple_list(self, ctx:PinDefinitionParser.Def_tuple_listContext):
        pass


    # Enter a parse tree produced by PinDefinitionParser#complex_member.
    def enterComplex_member(self, ctx:PinDefinitionParser.Complex_memberContext):
        pass

    # Exit a parse tree produced by PinDefinitionParser#complex_member.
    def exitComplex_member(self, ctx:PinDefinitionParser.Complex_memberContext):
        pass


    # Enter a parse tree produced by PinDefinitionParser#complex_member_list.
    def enterComplex_member_list(self, ctx:PinDefinitionParser.Complex_member_listContext):
        pass

    # Exit a parse tree produced by PinDefinitionParser#complex_member_list.
    def exitComplex_member_list(self, ctx:PinDefinitionParser.Complex_member_listContext):
        pass


    # Enter a parse tree produced by PinDefinitionParser#peripheral_def.
    def enterPeripheral_def(self, ctx:PinDefinitionParser.Peripheral_defContext):
        pass

    # Exit a parse tree produced by PinDefinitionParser#peripheral_def.
    def exitPeripheral_def(self, ctx:PinDefinitionParser.Peripheral_defContext):
        pass


