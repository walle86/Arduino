/*
  Temperaturlogger using an MCP3304 as ADC and saving data to an SD card
  
  I only have the standard TMP36 wich comes with the Arduino Starterkit.
  The time is since Startup of the Arduino
  
  programmed by Stefan Wallnoefer for josephchrzempiec
  more librarys and sketches under https://github.com/walle86/
*/

#include <SD.h>
#include <MCP3304.h>
#include <SPI.h>

//int analogPin = 0; dont need it because you use the MCP3304 as input
int numReadings = 10;  //make 10 redings to gest a stable value
int readValue = 0;
float average = 0;
float voltage = 0;
float temperature = 0;
float temperatureF = 0;
float offsetVoltage = 0.5; //outputvoltage of Sensor at 0°C -> 500mV for the tmp36

const int chipSelectSD = 10;
const int chipSelectMCP = 9;
const int errorPin = 8;

unsigned long time; // Variable to store the time (only since startup)
unsigned int wait = 1000; // How long to delay between sensor readings in milliseconds

MCP3304 adc(chipSelectMCP);  //creating the ADC

void setup() {

  Serial.begin(9600);
  Serial.print("Initializing SD card...");
  pinMode(chipSelectSD, OUTPUT);
  pinMode(errorPin, OUTPUT);  //connect LED wich will light up iff an error occurd
  digitalWrite(errorPin, LOW); 

  if (!SD.begin(chipSelectSD)) {
    Serial.println("Card failed, or not present");
    digitalWrite(errorPin, HIGH); 
    // don't do anything more:
    return;
  }

  Serial.println("card initialized.");
  
  //Write a Headerline in datalog.txt
  
  File dataFile = SD.open("datalog.txt", FILE_WRITE);
  if (dataFile) {
    dataFile.println("Programmed by Stefan Wallnoefer for josephchrzempiec\nmore librarys and sketches under https://github.com/walle86/\n");
    dataFile.println("The time is since Startup of the Arduino\n");
    dataFile.println("Time [s]\tTemperature [F]");
    dataFile.close();
  }
  else {
    Serial.println("error opening datalog.txt");
    digitalWrite(errorPin, HIGH); 
  } 

}

void loop() {
  
  if(millis() - time >= wait){
    time = millis();
    //read 10 values in about 100ms to get a stabler result
    for (int i = 0; i < numReadings; i++){
      readValue += adc.readSgl(0);
      delay(10);
    }
    voltage = readValue / numReadings * 5.0 / 4095.0; //average of 10 reading converted into a voltage
    temperature = (voltage - offsetVoltage) * 100;
    temperatureF = (temperature * 1.8) + 32;

    Serial.print("Temperature: ");
    Serial.print(temperatureF);
    Serial.println("F");
    
    // open the file. note that only one file can be open at a time,
    // so you have to close this one before opening another.
    File dataFile = SD.open("datalog.txt", FILE_WRITE);
    if (dataFile) {
      dataFile.print(round(time/1000.0));
      dataFile.print("\t");
      dataFile.println(temperatureF,1);
  
      dataFile.close();
    }
    // if the file isn't open, pop up an error:
    else {
      Serial.println("error opening datalog.txt");
      digitalWrite(errorPin, HIGH);
    }
    
    readValue = 0; //set value back
  }
}

