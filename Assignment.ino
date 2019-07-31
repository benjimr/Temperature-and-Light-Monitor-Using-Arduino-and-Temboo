#include <WiFi.h>
#include <WiFiClient.h>
#include <Temboo.h>
#include <LiquidCrystal.h>

#if TEMBOO_LIBRARY_VERSION < 2
#error "tlv error"
#endif

WiFiClient client;//used for connecting to the internet
String sheetID;//used to store the create sheet

//used to display messages based on sensor input
LiquidCrystal lcd(2, 3, 5, 6, 8, 9);

//timers
unsigned long updateSheetTimer = 0, updateLocalTimer = 0, emailStatusTimer = 0;

//store readings globally so it is easier to access
int light;
float temp;

//when data is recorded it is added to this string, uploaded every 15 seconds, and reset
String uploadData = "";

void setup() 
{
  pinMode(A0, INPUT);//light sensor
  pinMode(A1, INPUT);//temp sensor
  
  lcd.begin(16,2);//initialise the LCD display
  
  setupWifi();//connects to internet
  createSheet();//creates sheet and stores extracted ID in sheetID

  //reset timers so they start only when eveything is initialized
  updateLocalTimer = updateSheetTimer = emailStatusTimer = millis();
}

void loop()
{  
  //record data every 2.5 seconds
  if((unsigned long)(millis() - updateLocalTimer) >= 2500)
  {
    recordSensorData();//records the data and stores it in the global variables for it
    checkData();//based on the values stored in the global variables, update message on LCD display
    
    //reset timer
    updateLocalTimer = millis();
  }

  //upload data every 15 seconds
  if((unsigned long)(millis() - updateSheetTimer) >= 15000)
  {
    uploadData += "]";//close the json formatted string before uploading
    appendValues(uploadData);//upload the recorded values

    //reset timer and string for storing data
    updateSheetTimer = millis();
    uploadData = "";

  }

  //email data every 2 minutes
  if((unsigned long)(millis() - emailStatusTimer) >= 120000)
  {
    //simply send the latest readings to my email, can't really make a proper message because of limited ram
    sendEmail((String)light + " " + (String)temp);

    //reset timer
    emailStatusTimer = millis();
  }
}

//SENSOR DATA HANDLING

//simply records sensor data and stores it in global variables
void recordSensorData()
{
  //read data
  light = analogRead(A0);
  int tempReading = analogRead(A1);
  
  //convert data
  float voltage = (tempReading/1024.0) * 5.0;
  temp = voltage * 100.0;
}

//format the last recorded values as a single row in json
String getFormattedRow()
{
  String result = ",[\"" + (String)light + "\",\"" + (String)temp + "\"]";
  return result;
}

//check the last recorded values and set screen output based on them
void checkData()
{
  //if first append since last upload put headers on
  if(uploadData.equals(""))
  {
    uploadData += "[[\"\n\nLight\",\"\n\nTemp\"]";
  }

  //append the data to the string
  uploadData += getFormattedRow();

  //set light status
  //between 75 and 175 are ok values
  if(light > 75 && light < 175)
  {
    lcd.setCursor(0,1);
    lcd.print("Light:OK  ");
  }
  //below 75 is low
  else if(light < 75)
  {
    lcd.setCursor(0,1);
    lcd.print("Light:Low ");
  }
  //above 175 is high
  else
  {
    lcd.setCursor(0,1);
    lcd.print("Light:High");
  }

  //set temp status
  //between 20 and 27 are ok values
  if(temp > 20 && temp < 27)
  {    
    lcd.setCursor(0,0);
    lcd.print("Temp:OK  ");
  }
  //below 20 is low
  else if(temp < 20)
  {
    lcd.setCursor(0,0);
    lcd.print("Temp:Low ");
  }
  //above 27 is high
  else
  {
    lcd.setCursor(0,0);
    lcd.print("Temp:High");
  }
}

//CHOREOS

//create a spreadsheet on google sheets and retrieve its id
void createSheet()
{
  //setup choreo
  TembooChoreo choreo(client);
  choreo.begin();
  
  //Fill in using environment variables or header file for security reasons.
  choreo.setAccountName(<Account Name>);
  choreo.setAppKeyName(<App Name>);
  choreo.setAppKey(<Key>);
  choreo.setProfile(<Profile>);

  String properties = "{\"title\":\"SensorData\"}";
  
  choreo.addInput("SpreadsheetProperties", properties);
  choreo.setChoreo("/Library/Google/Sheets/CreateSpreadsheet");

  //run it
  choreo.run();

  //parse it
  String result = "";

  while(choreo.available())
  {
    //go as far as the string properties to ensure we have the id
    //but don't overflow the memory buffer
    if(result.indexOf("properties") != -1)
    {
      break;
    }
    else
    {
      char c = choreo.read();
      result += c;
    }
  }

   //extract the id from the response so we can use it to append data
  int pos = result.indexOf(": \"")+3;//find this expected outputs position, just before the id
  result = result.substring(pos, result.length());//using that position cut the string
  
  pos = result.indexOf("\",");//find position just after id
  result = result.substring(0, pos);//cut string again leaving only the id

  sheetID = result;//store id globally
  choreo.close();
}

//add values to the newly created sheet
void appendValues(String value)
{
  //setup choreo
  TembooChoreo AppendValuesChoreo(client);
  
  AppendValuesChoreo.begin();
  
  //Fill in using environment variables or header file for security reasons.
  AppendValuesChoreo.setAccountName(<Account Name>);
  AppendValuesChoreo.setAppKeyName(<App Name>);
  AppendValuesChoreo.setAppKey(<Key>);
  AppendValuesChoreo.setProfile(<Profile>);
  AppendValuesChoreo.addInput("Values", value);
  
  AppendValuesChoreo.addInput("SpreadsheetID", sheetID);
  AppendValuesChoreo.setChoreo("/Library/Google/Sheets/AppendValues");

  AppendValuesChoreo.run();

  //ignore output to save on ram and processing power
  //output not important

  AppendValuesChoreo.close();
}

//send an email with the last recorded sensor values
void sendEmail(String msg)
{
  //setup choreo
  TembooChoreo emailChoreo(client);

  emailChoreo.begin();
  
  //Fill in using environment variables or header file for security reasons.
  emailChoreo.setAccountName(<Account Name>);
  emailChoreo.setAppKeyName(<App Name>);
  emailChoreo.setAppKey(<Key>);

  emailChoreo.addInput("FromAddress", <From Email>);
  emailChoreo.addInput("ToAddress", <To Email>);
  emailChoreo.addInput("Subject", "ALERT");
  emailChoreo.addInput("MessageBody", msg);
  emailChoreo.setProfile(<Profile>);

  emailChoreo.setChoreo("/Library/Google/Gmail/SendEmail");

  //run the choreo
  emailChoreo.run();

  //ignoring output to save on processing power and ram, as it is quite limited
  //and the response is not hugely important
  emailChoreo.close();
}

//WIFI

//connect to wifi
void setupWifi()
{
  int wifiStatus = WL_IDLE_STATUS;

  // Determine if the WiFi Shield is present
  if (WiFi.status() == WL_NO_SHIELD)
  {
    // If there's no WiFi shield, stop here
    while(true);
  }

  // Try to connect to the local WiFi network
  while(wifiStatus != WL_CONNECTED) 
  {
	//Fill in using environment variables or header file for security reasons.
    wifiStatus = WiFi.begin(<SSID>, <Password>);
    delay(5000);
  }
}

