#ifndef Floatter_h
#define Floatter_h

#include <WiFiNINA.h>  
#include <PubSubClient.h>
#include <OneWire.h> 
#include <DallasTemperature.h>
#include <Adafruit_BMP085.h>
#include <ArduinoJson.h>

extern int SENDING_TIME;
extern PubSubClient client;

struct pos{
    float lat;
    float lon;
};

struct wave{
    char* name;
    float height;
    float period;
    float power;
};

struct sensData{
    char* name;
    float temp;
    float salinity;
    pos position;
};

void sensorSetup();
void wifiSetup();

void reconnect();
void send(char*, char*);

void sendWaveStat();
void sendLoop();

int getMediumNum(int[], int);

char* getName();
sensData getData();
float getTemperature();
float getSalinity();
pos getPosition();
wave getWaveStat();

#endif