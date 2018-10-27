

#include <SoftwareSerial.h> // needed to connect via serial port
SoftwareSerial EspSerial(10, 11); // Rx,  Tx
String errore =""; //string for error check across the libraries
String statusChWriteKey = "FXH3AE6AZNZGKYVG";  // Thing speak Status authorization code
String ThingSpeakString = ""; //variable to contain the data to be sent
bool debug = false; // debug off

#include <SimpleDHT.h> //library for the humidity and temperature sensor DHT
int pinDHT11 = 3; // need to define the digital pin to connect the single wire transmission, in this case DIG3
SimpleDHT11 dht11; //generate the object
int err = SimpleDHTErrSuccess;

int LightSensorAnalogPort = 0; //Analog port where the light sensor is connected

void setup() {
Serial.begin(9600); // open serial port, set the baud rate to 9600 bps
while (!Serial) {
    ;               // wait for serial port to connect. Needed for native USB port only
  }
EspSerial.begin(9600); // starting the communication with the ESP
}


void loop() {

byte Temperature = ReadTemperature();
byte Humidity = ReadHumidity();
float Light = ReadLight();\

StartTSstring(); //generate the general part of the string to be sent
AddTSstring("field1",String(Temperature));
AddTSstring("field2",String(Humidity));
AddTSstring("field3",String(Light));

//Serial.print(ThingSpeakString);
//Serial.println(),
SendTSstring();
delay (60000); //delay to repeat the command

}

/*Leo_ESP8266-1, 
ESP 8266 wifi communication with thingspeak ver 29-9-2018

Previously the ESP need to be set in:
AT+CIOBAUD=9600  to get the 9600 baud speed connection which seems the best one for this application
AT+CWMODE=1 to be a sta, used to send command 
AT+CWJAP="network_name","password" in case is not yet configured


Need the :                  
#include <SoftwareSerial.h> // needed to connect via serial port
SoftwareSerial EspSerial(10, 11); // Rx,  Tx
String errore =""; //string for error check across the libraries
String statusChWriteKey = "FXH3AE6AZNZGKYVG";  // Thing speak Status authorization code
String ThingSpeakString = ""; //variable to contain the data to be sent

Errors :
Not included errors for now.

Debug:
bool debug = false; // debug off
if debug = true  will print on the serial print all the data and errors.
*/ 

void StartTSstring(void)
{
 ThingSpeakString = "";                        //Clean it
 ThingSpeakString = "GET /update?api_key=";
 ThingSpeakString += statusChWriteKey;
}

void AddTSstring(String field, String data)
{
 ThingSpeakString += "&";
 ThingSpeakString += field;
 ThingSpeakString += "=";
 ThingSpeakString += data;
}

void SendTSstring(void)
{
EspSerial.flush();//wait that everything has been sent before starting again.
int count=0; // internal counter
  
String cmd = "AT+CIPSTART=\"TCP\",\"";
cmd += "api.thingspeak.com"; // Adress IP of api.thingspeak.com
cmd += "\",80"; // port
  
EspSerial.println(cmd); 
Serial.println(cmd);

do{       //await that the buffer is full with the answer
  delay(500);
  count++;
    if(count==10){
    Serial.print("AT+CIPSTART no communication");
    return;
    }
  Serial.print("AT+CIPSTART waiting to 10: "); 
  Serial.println(count);  
  }while (EspSerial.available() <=0);     
  
if(EspSerial.find("OK")){
  Serial.println("AT+CIPSTART OK");
  } else {
         Serial.println("AT+CIPSTART error");
         EspSerial.println("AT+CIPCLOSE");  //close the communication
         return;
  }
 
cmd = "AT+CIPSEND=";
cmd += String(ThingSpeakString.length()+2);
EspSerial.println(cmd);
Serial.println(cmd);
count=0; //reset the counter

do{       //await that the buffer is full with the answer
  delay(500);
  count++;
    if(count==10){
    Serial.print("AT+CIPSEND no communication");
    return;
    }
  Serial.print("AT+CIPSEND waiting to 10: "); 
  Serial.println(count);  
  }while (EspSerial.available() <=0);

if(EspSerial.find("OK")){
  Serial.println("AT+CIPSEND OK");
  } else {
          Serial.println("AT+CIPSEND error");
          EspSerial.println("AT+CIPCLOSE");  //close the communication
          return;
  }

delay(2000);
EspSerial.println(ThingSpeakString);
Serial.println(ThingSpeakString);

count=0; //reset the counter

do{       //await that the buffer is full with the answer
  delay(500);
  count++;
    if(count==10){
    Serial.print("GET communication not working");
    return;
    }
  Serial.print("GET waiting to 10: "); 
  Serial.println(count);  
  }while (EspSerial.available() <=0);

if(EspSerial.find("SEND")){
  Serial.println("GET OK");
  } else {
          Serial.println("GET error");
          EspSerial.println("AT+CIPCLOSE");  //close the communication
          return;
  } 

EspSerial.println("AT+CIPCLOSE");

}

/*Leo_LightSensor-2, 
Light sensor function ver 15-10-2018 give the results in lux

Need the :                  
no libraries to include
String errore =""; //string for error check across the libraries
int LightSensorAnalogPort = 0; //Analog port where the light sensor is connected

Errors :
Not included errors for now.

Debug:
bool debug = false; // debug off

if debug = true  will print on the serial print all the data and errors.

 */

int ReadLight(){

float val; // internal variable
float lux; // lux variable according the sensor calibration.
  val=analogRead(LightSensorAnalogPort);
    if(val<=850) lux=0.956*pow(2.6,(0.0055*val));  //these two regression depends on the sensor calibration
    else lux=4.8803*val-4042.3;    
  if (debug) {
    Serial.print(val,DEC);
    Serial.print("  : ");
    Serial.println(lux,DEC);      
  }
  delay(80); //estimated delay to get another read from the sensor
  
return(lux);
}

/*Leo_DHT11-1, 
DHT11 function ver 24-9-2018

Need the :                  
#include <SimpleDHT.h> //library for the humidity and temperature sensor DHT
int pinDHT11 = 3; // need to define the digital pin to connect the single wire transmission, in this case DIG3.
SimpleDHT11 dht11; //generate the object from the class SimpleDHT.
int err = SimpleDHTErrSuccess;  //needed for check
String errore =""; //string for error check across the libraries

Errors :
dht11-01 error is = ????????????

Debug:
bool debug = false; // debug off

if debug = true  will print on the serial print all the data and errors.???????????

 */


byte ReadTemperature(){

byte temp;  //for internal use
  
if ((err = dht11.read(pinDHT11, &temp, NULL, NULL)) != SimpleDHTErrSuccess) {
    errore = "dht11-01";
    if (debug) Serial.print("Read DHT11 failed, err="); 
    if (debug) Serial.println(err);
    return(0); // return to the loop if error accour
    }    
if (debug){ 
  Serial.print("Temp: ");
  Serial.print(temp);
}

delay(1400); // Humidity sensor need 1 sec min refresh

return(temp);
}

byte ReadHumidity(){

byte hum;  //for internal use
  
if ((err = dht11.read(pinDHT11, NULL, &hum, NULL)) != SimpleDHTErrSuccess) {
    errore = "dht11-01";
    if (debug) Serial.print("Read DHT11 failed, err="); 
    if (debug) Serial.println(err);
    return(0); // return to the loop if error accour
    }    
if (debug){ 
  Serial.print("Hum: ");
  Serial.print(hum); 
}

delay(1400); // Humidity sensor need 1 sec min refresh

return(hum);
}


