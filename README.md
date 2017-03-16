# Solar Tracker
## Overview
This project was designed to improve the efficiency of a solar panel. Its designed to be placed on rooftops of moving machines like automobiles but can also be used on stationary machines as well. The bot would align the solar panel correctly based on time and date to improve the panel efficiency. The alignment is based on the sun's position which is calculated using Azimuthal Angle, Elevation Angle, Local Solar Time Local Time, Local Standard Time Meridian, Equation Of Time, Declination, Hour Angle and Time Correction Factor. These calculations were made with respect to a fixed latitude and longitude, a gps module can be connected if needed to obtain the co-ordinates in real time. More details of sun's position calculations can be found [here](http://www.pveducation.org/pvcdrom/2-properties-sunlight/solar-time).

## Components Used:
1. Real Time Clock Module(DS3231)
2. Magnetometer(HMC5883)
3. Arduino Nano
4. 5V Relay
5. Solar Panel
6. 12V Lead Acid Battery
7. 12V DC Motors
8. L298 Motor Driver
9. Voltage Regulators
10. Solar Charge Controller

## Model
![Model](/Images/SolarTracker.jpg?raw=true "Optional Title")

## Usage
Copy the files in library folder to the library folder of arduino. Run the Solar_Sketch.ino.

## References
1. [Real Time Clock Tutorial](http://www.instructables.com/id/Real-time-clock-using-DS3231-EASY/?ALLSTEPS)
2. [Magnetometer Tutorial](https://learn.adafruit.com/adafruit-hmc5883l-breakout-triple-axis-magnetometer-compass-sensor/wiring-and-test)
