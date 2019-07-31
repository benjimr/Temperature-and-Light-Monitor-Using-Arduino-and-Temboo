# Temperature and light monitor using Arduino and Temboo
Simple arduino device used to montior temperature and light levels in a room, recording readings to an automatically created spreadsheet on Google Sheets, as well as emailing results periodically. A LCD screen is also used to allow local monitoring of readings.

NOTE: Due to RAM limitations on the Arduino Uno (2KB) to get all the functions to run without using all the RAM and causing either a crash or strange activity,I was forced to try and save on RAM usage anywhere I could. This means I had to remove as many unnecessary variables as possible, meaning all the Choreo details are in the main file and I created profiles on Temboo to use instead of putting the google details in the code. I also had to remove Serial messages, so there is no output other than what’s on the LCD display. I also ignore the Choreo responses where possible.

## Steps in the process
1. Setup pins, LCDs, timers etc.
   
2. Connect to WIFI.
   
3. Create the spread sheet.

    This is done using the “CreateSpreadsheet” Choreo. Once the Choreo is setup with all the required data e.g. account details, profile, title etc. we can run it. After it has run, it will respond. From this response we extract ID and store it for later.

4. Record data every 2.5 seconds.

    Every 2.5 seconds we read from the sensors, format them as JSON data and append them to a string for uploading later.

5. Display a message on the LCD display based on the data recorded.
    
    After the data has been recorded we change the output on the display, either OK,low or high for both temperature and light.

6. Uploaded the recorded data every 15 seconds.

    Every 15 seconds we take the string that has been built up as JSON data and use the “AppendValues” Choreo along with the spreadsheet ID retrieved earlier to upload the data. Once uploaded we clear that string so it can start building it again. The response from this Choreo is ignored.

7. Email a recording.

    Every 2 minutes the latest sensor data is sent by email to an email account using the “SendEmail” Choreo. This was originally coded to only email the recipient if the readings exceeded specified limits, however this caused difficulty when trying to demonstrate this function under lab conditions and so was changed to a simple timer. 

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