 
grammar PinDefinition;

@header {
from pin_def import *
}

specification returns [spec]
  : el+=definition+ EOF {$spec = [x.e for x in $el]}
  ;

definition returns [e]
  : a=single_pin_def {$e = $a.pin }
  | a=peripheral_def {$e = $a.e }
  | a=timer_def {$e = $a.timer}
  | a=bus_def {$e = $a.bus}
  ;

bus_def returns [bus]
  : BUS '{' l=complex_member_list '}' {$bus = Bus(**$l.e)}
  ;

pins_def returns [pins]
  : PINS '{' l=pin_list '}' {$pins = {'pins':PinSet($l.pins)}}
  ;

pin_list returns [pins]
  : h=pin_def {$pins = [$h.pin]}
  | h=pin_def ',' t=pin_list {$pins = $t.pins + [$h.pin]}
  ;

single_pin_def returns [pin]
  : PIN p=pin_def {$pin = $p.pin}
  ;

pin_def returns [pin]
  : '{' l=def_tuple_list '}' {$pin = Pin(**$l.e) }
  ;

timer_def returns [timer]
  : TIMER '{' l=def_tuple_list '}' {$timer = Timer(**$l.e) }
  ;

def_tuple returns [e]
  : f = ID ':'  s = ID {$e = {$f.text:$s.text} }
  ;

def_tuple_list returns [e]
  : head=def_tuple {$e = $head.e }
  | head=def_tuple ',' tail=def_tuple_list {$head.e.update($tail.e); $e = $head.e}
  ;

complex_member returns [e]
  : v=def_tuple {$e = $v.e}
  | v=pins_def {$e = $v.pins}
  ;

complex_member_list returns [e]
  : head=complex_member {$e = $head.e}
  | head=complex_member ',' tail=complex_member_list {$head.e.update($tail.e); $e = $head.e}
  ;

peripheral_def returns [e]
  : PERIPHERAL '{' l=complex_member_list '}' {$e = Peripheral(**$l.e) }
  ;

PINS : 'pins' ;
PIN : 'pin' ;
TIMER : 'timer' ;
BUS : 'bus' ;
PERIPHERAL : 'peripheral' ;

fragment
LETTER
  : 'a' .. 'z'
  | 'A' .. 'Z'
  | '_'
  ;

fragment
DIGIT
  : '0' .. '9'
  ;


ID
  : (LETTER|DIGIT)+
  ;

KEY
  : 'name'
  | 'bus'
  | 'gpio'
  | 'index'
  | 'channel'
  ;

WS
: [ \t\r\n\f]+
-> channel(HIDDEN)
;

LINE_COMMENT
: '#' ~[\r\n]* -> channel(HIDDEN)
;

