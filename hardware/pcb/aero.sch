EESchema Schematic File Version 2
LIBS:aero
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:special
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:aero-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 2
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L CONN_01X09 P1
U 1 1 551C5BBD
P 7550 1650
F 0 "P1" H 7550 2150 50  0000 C CNN
F 1 "CONN_01X09" V 7650 1650 50  0000 C CNN
F 2 "Socket_Strips:Socket_Strip_Straight_1x09" H 7550 1650 60  0001 C CNN
F 3 "" H 7550 1650 60  0000 C CNN
	1    7550 1650
	-1   0    0    1   
$EndComp
$Comp
L Vreg_Header H1
U 1 1 551C5F7E
P 7550 2450
F 0 "H1" H 7500 2400 60  0000 C CNN
F 1 "Vreg_Header" V 7350 2400 60  0000 C CNN
F 2 "Socket_Strips:Socket_Strip_Straight_1x02" H 7550 2450 60  0001 C CNN
F 3 "" H 7550 2450 60  0000 C CNN
	1    7550 2450
	1    0    0    -1  
$EndComp
Wire Wire Line
	7800 2300 8000 2300
Wire Wire Line
	8000 2400 7800 2400
$Sheet
S 8000 1150 1950 1350
U 551C09E8
F0 "nRF8001" 60
F1 "nrf8001.sch" 60
F2 "Vcc" I L 8000 2300 60 
F3 "Gnd" I L 8000 2400 60 
F4 "MOSI" I L 8000 1750 60 
F5 "MISO" O L 8000 1850 60 
F6 "~RDYN" O L 8000 1950 60 
F7 "~REQN" I L 8000 1650 60 
F8 "SCK" I L 8000 1550 60 
F9 "~RESET" I L 8000 2050 60 
F10 "ACTIVE" I L 8000 1250 60 
F11 "TXD" I L 8000 1350 60 
F12 "RXD" I L 8000 1450 60 
$EndSheet
Wire Wire Line
	7750 1250 8000 1250
Wire Wire Line
	8000 1350 7750 1350
Wire Wire Line
	7750 1450 8000 1450
Wire Wire Line
	8000 1550 7750 1550
Wire Wire Line
	8000 1650 7750 1650
Wire Wire Line
	7750 1750 8000 1750
Wire Wire Line
	8000 1850 7750 1850
Wire Wire Line
	7750 1950 8000 1950
Wire Wire Line
	8000 2050 7750 2050
$EndSCHEMATC
