EESchema Schematic File Version 2
LIBS:aero-rescue
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
Sheet 8 8
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
L U.FL_Antenna A2
U 1 1 55B6C93F
P 7075 3950
F 0 "A2" H 6925 4150 60  0000 C CNN
F 1 "U.FL_Antenna" V 7200 4250 60  0000 C CNN
F 2 "aero:CGGP.25.4.A.02" H 7075 3950 60  0001 C CNN
F 3 "" H 7075 3950 60  0000 C CNN
	1    7075 3950
	-1   0    0    -1  
$EndComp
$Comp
L GND #PWR055
U 1 1 55B7085D
P 5925 4575
F 0 "#PWR055" H 5925 4325 50  0001 C CNN
F 1 "GND" H 5925 4425 50  0000 C CNN
F 2 "" H 5925 4575 60  0000 C CNN
F 3 "" H 5925 4575 60  0000 C CNN
	1    5925 4575
	1    0    0    -1  
$EndComp
Text HLabel 4325 4000 0    60   Input ~ 0
TIMEPULSE
Text HLabel 4325 3700 0    60   Input ~ 0
TxD
Text HLabel 4325 3625 0    60   Input ~ 0
RxD
Text HLabel 4325 3775 0    60   Input ~ 0
SDA
Text HLabel 4325 3850 0    60   Input ~ 0
SCL
Text HLabel 4325 3925 0    60   Input ~ 0
EXT_INT
Text HLabel 4325 3550 0    60   Input ~ 0
RESET
$Comp
L +3V #PWR056
U 1 1 55B708B7
P 4425 4275
F 0 "#PWR056" H 4425 4175 60  0001 C CNN
F 1 "+3V" H 4275 4425 60  0000 C CNN
F 2 "" H 4425 4275 60  0000 C CNN
F 3 "" H 4425 4275 60  0000 C CNN
	1    4425 4275
	1    0    0    -1  
$EndComp
$Comp
L u-blox_max-7c/q U13
U 1 1 55BA51BB
P 5200 4075
F 0 "U13" H 5175 4425 60  0000 C CNN
F 1 "u-blox_max-7c/q" H 5350 3525 60  0000 C CNN
F 2 "aero:ublox_MAX7" H 5125 4100 60  0001 C CNN
F 3 "" H 5125 4100 60  0000 C CNN
	1    5200 4075
	1    0    0    -1  
$EndComp
Wire Wire Line
	4600 3550 4325 3550
Wire Wire Line
	4325 3625 4600 3625
Wire Wire Line
	4600 3700 4325 3700
Wire Wire Line
	4325 3775 4600 3775
Wire Wire Line
	4325 3850 4600 3850
Wire Wire Line
	4325 3925 4600 3925
Wire Wire Line
	4325 4000 4600 4000
Wire Wire Line
	4600 4350 4425 4350
Wire Wire Line
	4425 4275 4425 4500
Wire Wire Line
	4425 4425 4600 4425
Connection ~ 4425 4350
Wire Wire Line
	4425 4500 4600 4500
Connection ~ 4425 4425
Wire Wire Line
	5775 4350 5925 4350
Wire Wire Line
	5925 4350 5925 4575
Wire Wire Line
	5775 4425 5925 4425
Connection ~ 5925 4425
Wire Wire Line
	5775 4525 5925 4525
Connection ~ 5925 4525
Wire Wire Line
	5775 4075 7075 4075
$Comp
L CONN_01X03 P1
U 1 1 5612ECE3
P 4550 2025
F 0 "P1" H 4550 2225 50  0000 C CNN
F 1 "CONN_01X03" V 4650 2025 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x03" H 4550 2025 60  0001 C CNN
F 3 "" H 4550 2025 60  0000 C CNN
	1    4550 2025
	0    -1   -1   0   
$EndComp
Wire Wire Line
	4450 2225 4450 3625
Connection ~ 4450 3625
$Comp
L GND #PWR057
U 1 1 5612EDCC
P 4650 2350
F 0 "#PWR057" H 4650 2100 50  0001 C CNN
F 1 "GND" H 4650 2200 50  0000 C CNN
F 2 "" H 4650 2350 60  0000 C CNN
F 3 "" H 4650 2350 60  0000 C CNN
	1    4650 2350
	1    0    0    -1  
$EndComp
NoConn ~ 5775 3575
NoConn ~ 5775 3975
Wire Wire Line
	7075 4075 7075 3950
Wire Wire Line
	4550 2225 4550 3700
Connection ~ 4550 3700
Wire Wire Line
	4650 2225 4650 2350
$EndSCHEMATC
