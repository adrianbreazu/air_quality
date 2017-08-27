#include <Wire.h>
#define I2CAddressESPWifi 8

const int UV_OUT  = A5;
const int UV_REF  = A4;
const int MQ7     = A3;
const int MQ3     = A2; 
const int MQ2     = A1;
const int MQ135   = A0;

String sensor;
char response_data[7];
int uvLevel=0;
int refLevel=0;

void setup()
{
  Serial.begin(115200);
  pinMode(UV_OUT, INPUT);
 
  Wire.begin(I2CAddressESPWifi);
  Wire.onReceive(espReceiveEvent);
  Wire.onRequest(espRequestEvent);
  
  Serial.println("finshed setup");
}

void loop()
{
  delay(1);
}

int averageAnalogRead(int pinToRead)
{
  byte numberOfReadings = 8;
  unsigned int runningValue = 0; 

  for(int x = 0 ; x < numberOfReadings ; x++)
    runningValue += analogRead(pinToRead);
  runningValue /= numberOfReadings;

  return(runningValue);  
}

//The Arduino Map function but for floats
//From: http://forum.arduino.cc/index.php?topic=3922.0
float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


void espReceiveEvent(int count)
{
  sensor = "";
  //Serial.print("Received[");
  while (Wire.available()) {
    char c = Wire.read();
    //Serial.print(c);
    sensor.concat(c);
  }
  //Serial.println("]");
  Serial.print("ESP request for sensor: ");
  Serial.println(sensor);
  Serial.println("-----------espReceiveEvent-------------");
}

void espRequestEvent()
{
  int data;
  memset(response_data, 0, sizeof(response_data));
  
  //Serial.println(sensor);
  if (sensor != "") {
    if (sensor == "MQ2") {
      data = analogRead(MQ2);
      sprintf(response_data, "%07d", data);
      Wire.write(response_data);
      Serial.print("MQ2 - sending response data value: ");
      Serial.println(response_data);
    }
    else if (sensor == "MQ3") {
      data = analogRead(MQ3);
      sprintf(response_data, "%07d", data);
      Wire.write(response_data);
      Serial.print("MQ3 - sending response data value: ");
      Serial.println(response_data);
    }
    else if (sensor == "MQ7") {
      data = analogRead(MQ7);
      sprintf(response_data, "%07d", data);
      Wire.write(response_data);
      Serial.print("MQ7 - sending response data value: ");
      Serial.println(response_data);
    }
    else if (sensor == "MQ135") {
      data = analogRead(MQ135);
      sprintf(response_data, "%07d", data);
      Wire.write(response_data);
      Serial.print("MQ135 - sending response data value: ");
      Serial.println(response_data);
    }
    else if (sensor == "UV") {
      uvLevel = averageAnalogRead(UV_OUT);
      refLevel = averageAnalogRead(UV_REF);      
      //Use the 3.3V power pin as a reference to get a very accurate output value from sensor
      float outputVoltage = 3.3 / refLevel * uvLevel;
      float uvIntensity = mapfloat(outputVoltage, 0.99, 2.8, 0.0, 15.0); //Convert the voltage to a UV intensity level
      //convert data to integer and revert on ESP as there is no sprintf for float;
      data = uvIntensity * 100;
      sprintf(response_data, "%07d", data);
      Wire.write(response_data);
      Serial.print("UV - sending response data value: ");
      Serial.println(response_data);
    }
    else {
      Serial.print("Unmapped request sent: ");
      Serial.println(sensor);
    }
  }
  else
    Serial.println("Request is empty :( unable to process request");
    
  Serial.println("-----------espRequestEvent-------------");
  Serial.println("");
}
