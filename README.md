# Macbook Battery Model A1185 data read

Read the MacBook White (A1181) Internal Battery Texas Instrumens **DQ20Z80** IC using an Arduino Leonardo.

## Battery Information

![](img/a1185.png)

## Battery Board Gutshot
![](img/A1181-battery-board.jpg)

## Battery Connector PinOut

![](img/A1185_pinout.png)

 Pin       | Description          
| ------------- |:-------------:
| POS     | Positive +
| TERM    | Internal Thermistor     
| SCL     | I2C Clock      
| SDA     | I2C Data  
| SW      | Internal Switch 
| GND     | Ground  

## Battey Information Terminal Output
![](img/terminal_output.png)

## Bench Setup

 Battery      | Arduino          
| ------------- |:-------------:
| POS     | NC
| TERM    | NC     
| SCL     | I2C SCL
| SDA     | I2C SDA  
| SW      | Pin 11 
| GND     | GND 

![](img/bench_1.jpg)
![](img/bench_2.jpg)
![](img/bench_3.jpg)
