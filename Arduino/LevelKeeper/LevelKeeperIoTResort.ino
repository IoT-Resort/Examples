/*
  LevelKeeperIotResort.cpp - Arduino example for IoT-Resort platform.
  Created by IoT-Resort, November 23, 2017. Ver. 2.0
  Released into the public domain.
*/
#include <ESP8266WiFi.h>
#include <IoT-Resort.h>

//точка доступа и пароль WiFi
const char* ssid     = "???";
const char* password = "???";

unsigned long CurrentMoment;
unsigned long DataExchangeMoment; // момент последнего обмена данными в ms с начала работы
unsigned long DataExchange = 5000L;  // частота обмена данными в ms

unsigned long SensorCheckMoment; // момент последнего опроса сенсора в ms с начала работы
unsigned long SensorCheck = 1000L; // частота опроса сенсора в ms

//пины подключение реле и сенсора
int RelayPin = 2;
int SensorPin = 0;

WiFiClient client;

//сервер IoT-платформы, порт доступа и серийный номер устройства
Thing LevelKeeper("www.iot-resort.ru", 80, "303C07B4399C4B3D9AE556FFB5F78350", PROTOCOL_HTTP, &client );

int SensorValue;
boolean RelayOn = false;

void setup() {
    pinMode(RelayPin, OUTPUT);
    digitalWrite(RelayPin, LOW);
    pinMode(SensorPin, INPUT);

  
    Serial.begin(115200);
    delay(10);

    // соединение с WiFi точкой

    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");  
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    DataExchangeMoment=millis();
    SensorCheckMoment=millis();     
}



void loop() {
    
    CurrentMoment = millis();
    if (CurrentMoment < DataExchangeMoment || CurrentMoment < SensorCheckMoment ) // было обнуление millis() после 50 дней работы
    {      
        DataExchangeMoment=CurrentMoment;  
        SensorCheckMoment=CurrentMoment;  
    };

    if (CurrentMoment > SensorCheckMoment+SensorCheck ) // Get sensor value
    {
        
        SensorCheckMoment=CurrentMoment;
        SensorValue=digitalRead(SensorPin);
    };
    
    if (CurrentMoment > DataExchangeMoment+DataExchange ) // Get control parameters
    {
        DataExchangeMoment=CurrentMoment;
        if ( SensorValue == 0 ) 
        {
            LevelKeeper.SetIoTValue("TankIsFull","true");
        } else
        {
            LevelKeeper.SetIoTValue("TankIsFull","false");  
        }
        
        if ( LevelKeeper.GetIoTControlParameter("Valve") == "true")
        {
            RelayOn=true;
        } else
        {
            RelayOn=false;
        }
    };
    if (RelayOn==true && SensorValue==1){
        digitalWrite(RelayPin, HIGH);  
    }else
    {
        digitalWrite(RelayPin, LOW);  
    }
  
}

