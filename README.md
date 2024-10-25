# pc-fan-control-and-temperature-monitor
Uses the arduino PWM to control the speed of a 2 pin 120mm fan (using an N channel MOSFET) according to your CPU temperature. Additionally, a python script which uses the web hosting service of LibreHardwareMonitor to get the the locally hosted JSON data and extract real time CPU and GPU temps to display them on a 1602 LCD.
Features like CPU and GPU temps display on the LCD and fan speed percentage display.
Gradual fan PWM value increase and delay to check if temperature range has changed.
