#include "floatter_lib.h"

const int TEMP_PIN = 2;
const int SALINITY_PIN = A1;
int SENDING_TIME = 15*60*1000; //30min = (24*60*60*1000)s
const int MQTT_PORT = 1883;

Adafruit_BMP085 baro;

const char WIFI_SSID[] = "";
const char WIFI_PASS[] = "";

WiFiClient wifi;
IPAddress server(167, 99, 33, 14);
PubSubClient client(wifi);
char topic[] = "/floatter/data";
char topic1[] = "/floatter/wave";

OneWire oneWire(TEMP_PIN);
DallasTemperature temp_sensor(&oneWire);

void sensorSetup(){
    temp_sensor.begin();
    pinMode(SALINITY_PIN, INPUT);
    if(!baro.begin()){
        Serial.println("Failed to initialize BARO!");
    }
}

void wifiSetup(){
    uint32_t startMillis;
    uint32_t currentMillis;
    const uint32_t period = 500;
    int status = WL_IDLE_STATUS;

    while (status != WL_CONNECTED) {
        currentMillis = millis();
        if(currentMillis - startMillis >= period){
            status = WiFi.begin(WIFI_SSID, WIFI_PASS);
            startMillis = currentMillis;
        }
    }

    client.setServer(server, MQTT_PORT);
    client.setKeepAlive(6*60*60);

    if (client.connect("arduinoClient")) {
      client.subscribe(topic);
      client.publish("outTopic","hello");
    } else {
      Serial.println("mqtt not connected");
      Serial.print("failed, rc=");
      Serial.println(client.state());
    }
}

float getTemperature(){
    temp_sensor.requestTemperatures();
    return temp_sensor.getTempCByIndex(0);
}

pos getPosition(){
    return {44.389291, 8.976095};
}

int getMedianNum(int bArray[], int iFilterLen){
    int bTab[iFilterLen];
    for (uint8_t i = 0; i<iFilterLen; i++)
      bTab[i] = bArray[i];
    int i, j, bTemp;
    for (j = 0; j < iFilterLen - 1; j++)
    {
      for (i = 0; i < iFilterLen - j - 1; i++)
      {
        client.loop();
        if (bTab[i] > bTab[i + 1])
        {
          bTemp = bTab[i];
          bTab[i] = bTab[i + 1];
          bTab[i + 1] = bTemp;
        }
      }
    }
    if ((iFilterLen & 1) > 0)
      bTemp = bTab[(iFilterLen - 1) / 2];
    else
      bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
    return bTemp;
}

float getSalinity(){
    uint32_t startMillis;
    uint32_t currentMillis;
    const uint32_t period = 40;
    const int VREF = 3.3;
    const int NUM_POINTS = 30;
    float averageVoltage = 0;
    float tdsValue = 0;
    int i = 0;
    float temperature = getTemperature();

    int analogBuffer[NUM_POINTS];

    while(i < NUM_POINTS){
        currentMillis = millis();
        if (currentMillis - startMillis >= period){
            analogBuffer[i] = analogRead(SALINITY_PIN);
            startMillis = currentMillis;
            i++;
        }
    }

    averageVoltage = getMedianNum(analogBuffer,NUM_POINTS) * (float)VREF/ 1024.0;
    float compensationCoefficient=1.0+0.02*(temperature-25.0);
    float compensationVolatge=averageVoltage/compensationCoefficient;
    tdsValue=(133.42*compensationVolatge*compensationVolatge*compensationVolatge - 255.86*compensationVolatge*compensationVolatge + 857.39*compensationVolatge)*0.5;

    return tdsValue;
}

wave getWaveStat(){
    long pressure;
    float height, period, power, altitude, min_height, max_height, mid_point, smudge_factor;
    uint8_t escaped, started;
    uint32_t period_start, period_end;
    float avg_period = -1;
    char* name = getName();

    uint32_t currentMillis;
    uint32_t startMillis = millis();
    pressure = baro.readPressure();
    altitude = baro.readAltitude(pressure);
    max_height = altitude;
    min_height = altitude;
    
    while(millis() - startMillis < 15000){
      client.loop();
      pressure = baro.readPressure();
      altitude = baro.readAltitude(pressure);
      if (altitude < min_height) min_height = altitude;
      if (altitude > max_height) max_height = altitude;
    }

    mid_point = (max_height + min_height)/2.0;
    height = (max_height - mid_point) / 2.0;
    smudge_factor = height * 0.15;

    startMillis = millis();
    while(millis() - startMillis < 15000){
      client.loop();
      pressure = baro.readPressure();
      altitude = baro.readAltitude(pressure);

      if(altitude < mid_point + smudge_factor && altitude > mid_point - smudge_factor){
        if(!started){
          period_start = millis();
          started = true;
        }else{
          if(escaped){
            period_end = millis();
            started = false;
            escaped = false;

            if(avg_period != -1){
              avg_period = (avg_period + (period_end-period_start)*2)  / 2.0;
            }else{
              avg_period =  (period_end-period_start)*2; 
            }
          }
        }
      }else{
        escaped = true;
      }
    }

    period = avg_period/1000;
    power = int(0.5 * height * height * avg_period / 1000);

    return {name, height, period, power};
}

char* getName(){
    return "Flotty";
}

sensData getData(){
    char* name = getName();
    float temp = getTemperature();
    float salinity = getSalinity();
    pos position = getPosition();

    return {name, temp, salinity, position};
}

void reconnect() {
  uint32_t startMillis;
  uint32_t startMillis1;
  uint32_t currentMillis;
  const uint32_t period = 500;
  int status = WL_IDLE_STATUS;
  
  while (!client.connected()) {
    while (status != WL_CONNECTED) {
      currentMillis = millis();
      if(currentMillis - startMillis >= period){
          status = WiFi.begin(WIFI_SSID, WIFI_PASS);
          startMillis = currentMillis;
      }
    }
    Serial.print("Attempting MQTT connection...");
    if (client.connect("arduinoClient")) {
      Serial.println("connected");
      client.publish("outTopic","hello world");
      client.subscribe(topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      startMillis = millis();
      while(millis() - startMillis1 < 1000);
    }
  }
}

void send(char* data, char* topic){
    if (!client.connected() || WiFi.status()!=0) {
      reconnect();
    }

    if(!client.publish(topic, data, 128))
      Serial.print("not published\n");
}

void sendWaveStat(){
    client.loop();
    wave data = getWaveStat();
    StaticJsonDocument<128> doc;
    char buffer[128];

    JsonObject wave = doc.createNestedObject("wave");
    JsonObject boa = doc.createNestedObject("boa");
    boa["name"] = data.name;
    wave["height"] = data.height;
    wave["period"] = data.period;
    wave["power"] = data.power;

    serializeJson(doc, buffer);
    
    send(buffer, topic1);
}

void sendLoop(){
    client.loop();
    sensData data = getData();
    StaticJsonDocument<128> doc;
    char buffer[128];

    JsonObject boa = doc.createNestedObject("boa");
    boa["name"] = data.name;
    boa["lat"] = data.position.lat;
    boa["lon"] = data.position.lon;
    doc["temperature"] = data.temp;
    doc["salinity"] = data.salinity;

    serializeJson(doc, buffer);

    send(buffer, topic);
}