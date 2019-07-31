# Temperature-and-light-monit-using-Arduino-and-Temboo
Simple arduino device used to montior temperature and light levels in a room, recording readings to an automatically created spreadsheet on Google Sheets, as well as emailing results periodically. A LCD screen is also used to allow local monitoring of readings.

## Schematic

![Schematic](https://imgur.com/lTzGeVS.jpg "Schematic")

## Breadboard
### Diagram
![Breadboard](https://imgur.com/aKFxLDu.jpg "Breadboard")
### Actual
![Breadboard](https://imgur.com/sbq7BbM.jpg "Breadboard")
1. Photoresistor (LDR)
This is used to read brightness values. It works by varying resistance based on light. It inputs its value to pin A0.
2. Temperature Sensor (LM35)
This is used to read temperature values. It inputs its values from its center pin to pin A1.
3. Potentiometer
This is used to vary contrast on the LCD display. Its output is directed in to pin 3 of the LCD display
4. LCD Display
This is used to display messages based on the readings of the sensors. ( E.g. Temp : OK Light: High)



NOTE: Due to RAM limitations on the Arduino Uno (2KB) to get all the functions to run without using all the RAM and causing either a crash or strange activity,I was forced to try and save on RAM usage anywhere I could. This means I had to remove as many unnecessary variables as possible, meaning all the Choreo details are in the main file and I created profiles on Temboo to use instead of putting the google details in the code. I also had to remove Serial messages, so there is no output other than what’s on the LCD display. I also ignore the Choreo responses where possible.
