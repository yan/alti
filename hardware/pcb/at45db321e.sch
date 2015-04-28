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
Sheet 5 7
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
L AT45DB321E U7
U 1 1 552F3DBF
P 5450 3700
F 0 "U7" H 5600 2950 60  0000 C CNN
F 1 "AT45DB321E" H 5400 4350 60  0000 C CNN
F 2 "" H 5500 3350 60  0000 C CNN
F 3 "" H 5500 3350 60  0000 C CNN
	1    5450 3700
	1    0    0    -1  
$EndComp
$Comp
L +3V #PWR40
U 1 1 552F3DDF
P 5450 2750
F 0 "#PWR40" H 5450 2650 60  0001 C CNN
F 1 "+3V" H 5300 2900 60  0000 C CNN
F 2 "" H 5450 2750 60  0000 C CNN
F 3 "" H 5450 2750 60  0000 C CNN
	1    5450 2750
	1    0    0    -1  
$EndComp
Wire Wire Line
	5450 3200 5450 2750
$Comp
L GND #PWR41
U 1 1 552F3DF8
P 5450 4850
F 0 "#PWR41" H 5450 4600 50  0001 C CNN
F 1 "GND" H 5450 4700 50  0000 C CNN
F 2 "" H 5450 4850 60  0000 C CNN
F 3 "" H 5450 4850 60  0000 C CNN
	1    5450 4850
	1    0    0    -1  
$EndComp
Wire Wire Line
	5450 4850 5450 4300
Text HLabel 4700 3650 0    60   Input ~ 0
MOSI
Text HLabel 4700 3750 0    60   Input ~ 0
MISO
Wire Wire Line
	5000 3650 4700 3650
Wire Wire Line
	4700 3750 5000 3750
Text HLabel 4700 3850 0    60   Input ~ 0
SCK
Wire Wire Line
	4700 3850 5000 3850
Wire Wire Line
	5900 3850 6050 3850
Wire Wire Line
	6050 3850 6050 2900
Wire Wire Line
	6050 2900 5450 2900
Connection ~ 5450 2900
Text HLabel 6350 3750 2    60   Input ~ 0
~CS
Wire Wire Line
	6350 3750 5900 3750
Text HLabel 6350 3650 2    60   Input ~ 0
~RESET
Wire Wire Line
	6350 3650 5900 3650
$Comp
L C C25
U 1 1 552F40C6
P 4150 3050
F 0 "C25" H 4175 3150 50  0000 L CNN
F 1 "100nF" H 4175 2950 50  0000 L CNN
F 2 "" H 4188 2900 30  0000 C CNN
F 3 "" H 4150 3050 60  0000 C CNN
	1    4150 3050
	1    0    0    -1  
$EndComp
Wire Wire Line
	4150 2900 4150 2800
Wire Wire Line
	4150 2800 5450 2800
Connection ~ 5450 2800
Wire Wire Line
	4150 3200 4150 4500
Wire Wire Line
	4150 4500 5450 4500
Connection ~ 5450 4500
$EndSCHEMATC
