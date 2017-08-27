#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Wire.h>

#define I2CAddressESPWifi 8

String url  = "url...here..../sensors_actuators/store_data/";
char *ssid = "ssid....";
char *pwd  = "password....";

void setup()
{
  Serial.begin(115200);
  Wire.begin(0,2);//Change to Wire.begin() for non ESP.
  Wire.setClockStretchLimit(15000);

  WiFi.begin(ssid, pwd);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  while(WiFi.status() != WL_CONNECTED) {
  delay(1000);
  Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  delay(1000);
  Serial.println(WiFi.localIP());

}

void sendRequest(char *sensor) {
  //Serial.print("Connect to I2C adderss: ");
  //Serial.println(I2CAddressESPWifi);
  Wire.beginTransmission(I2CAddressESPWifi);
  Wire.write(sensor);
  Wire.endTransmission();  
}

String createPOSTMessage (String sensor, String data) {
  String json = "";
  
  if (sensor != "") {
    json = "{ \"pin\": \"";
    if (sensor == "MQ2") {
      json += "1234";
      json += "\", \"key\": \"";
      json += "ABCD";
      json += "\", \"value\": \"";
      json += data.toInt();
      json += "\", \"type\": \"";
      json += "MQ2";  
      json += "\" }";  
    }
    else if (sensor == "MQ3") {
      json += "12345";
      json += "\", \"key\": \"";
      json += "ABCDE";
      json += "\", \"value\": \"";
      json += data.toInt();
      json += "\", \"type\": \"";
      json += "MQ3";
      json += "\" }";
    }
    else if (sensor == "MQ7") {
      json += "123456";
      json += "\", \"key\": \"";
      json += "ABCDEF";
      json += "\", \"value\": \"";
      json += data.toInt();
      json += "\", \"type\": \"";
      json += "MQ7";
      json += "\" }";
    }
    else if (sensor == "MQ135") {
      json += "123";
      json += "\", \"key\": \"";
      json += "ABC";
      json += "\", \"value\": \"";
      json += data.toInt();
      json += "\", \"type\": \"";
      json += "MQ135";
      json += "\" }";
    }
    else if (sensor == "UV") {
      json += "321";
      json += "\", \"key\": \"";
      json += "CBA";
      json += "\", \"value\": \"";
      json += float(data.toInt())/100.00;
      json += "\", \"type\": \"";
      json += "UV";
      json += "\" }";
    }    
  }
  return json;
}

void sendMessage (String JSONMessage) {
   if (JSONMessage != "") {
     if(WiFi.status()== WL_CONNECTED){
        HTTPClient http;
        http.begin(url);
        http.addHeader("Content-Type", "application/json");
        int httpCode = http.POST(JSONMessage);
        String response = http.getString();
  
        Serial.println("JSON Request data:");
        Serial.println(JSONMessage);
        Serial.println("JSON Response data: ");
        Serial.println(response);
  
        http.end();
        delay(500);
        Serial.println("close connection");
      }else{
        Serial.println("error in WiFi connection");
      }
   } else
    Serial.println("JSON Requerst is empty. Cancel POST action");
}

void sendPOSTRequest(String sensor) {
  String data = "";
  String JSONResponse = "";
  
  Wire.requestFrom(I2CAddressESPWifi,7);
  while (Wire.available())
  {
    delay(1);
    char c = Wire.read();
    data.concat(c);
  }
  Serial.print("Create post request with value: ");
  Serial.println(data);
  JSONResponse = createPOSTMessage(sensor, data);
  Serial.println(JSONResponse);
  sendMessage(JSONResponse);
}

void requestSensorData(char *sensor) {
  Serial.print("Sent read request for sensor: ");
  Serial.println(sensor);
  sendRequest(sensor);
  
  delay(1);//Wait for Slave to calculate response.
  
  sendPOSTRequest(sensor);  
  Serial.print("Finalized POST request for sensor: ");
  Serial.println(sensor);
  delay(1000);
}

void loop()
{
  Serial.println("init loop");
  requestSensorData("MQ2");
  requestSensorData("MQ3");
  requestSensorData("MQ7");
  requestSensorData("MQ135");
  requestSensorData("UV");
  Serial.println("-----------loop-------------");
  Serial.println();
  //delay(3600000);
  delay(30000);
}


