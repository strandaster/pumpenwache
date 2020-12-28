

#include <Arduino.h>
#include <ACS712.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <SPI.h>
#include <BME280I2C.h>
#include <ArduinoOTA.h>
#include <TelnetSpy.h>
#include <ESP8266HTTPClient.h>
#include <pumpenwache.h>


TelnetSpy SerialAndTelnet;
#define SERIAL SerialAndTelnet


boolean pumpeAktiv = false;

// variables for timestamps
unsigned long lastEnvTimeStamp=0;
unsigned long lastACTimeStamp=0;
unsigned long lastCalTimeStamp=0;
unsigned long time_now;

// variables for environment
float globalTemp=0;
float globalPressure=0;
float globalHumidity=0;
float currentAC = 0;

int calValue=0;

ACS712 sensor(ACS712_20A, ACS712PIN);

// BME280 config
 BME280I2C::Settings settings(
   BME280::OSR_X1,
   BME280::OSR_X16,
   BME280::OSR_X1,
   BME280::Mode_Forced,
   BME280::StandbyTime_1000ms,
   BME280::Filter_Off,
   BME280::SpiEnable_False,
   BME280I2C::I2CAddr_0x76 // I2C address. I2C specific.
);
BME280I2C bme(settings); 

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
  WiFi.hostname(HOSTNAME);
  WiFi.persistent(false);
  WiFi.begin(SSID, WLAN_KEY);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  };
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  initBME();
  initArduinoOTA();
  SERIAL.begin(115200);

  sensor.setVoltageReference(VOLTAGE_REFERENCE_VALUE);

  calValue = sensor.calibrate();
  Serial.print("Calibration value: ");
  Serial.println(calValue);

  sensor.setZeroPoint(ACS_CALIBRATION_VALUE);

  //set value to negative to get a current value during start up
  lastEnvTimeStamp = millis() - TIMER_INTERVAL_UPDATE_ENV;
}

void loop() {
 time_now = millis();

if (checkTimer(time_now, lastEnvTimeStamp, TIMER_INTERVAL_UPDATE_ENV)) {
  // it is time for getting environment values like temperature
  readEnvValues();
  lastEnvTimeStamp=time_now;

  sendEnvToDB();
} 

if (checkTimer(time_now, lastACTimeStamp, TIMER_INTERVAL_AC)) {
  // check AC usage
  SERIAL.printf("Cal Value: %i \n", calValue);
  currentAC = readACValue();
  SERIAL.printf("get AC current: %f \n",currentAC);
  SERIAL.printf("current W: %f \n", currentAC*230);

  float currentVoltage = sensor.getVoltage();
  SERIAL.printf("current Voltage: %f \n", currentVoltage);

  float currentDC = sensor.getCurrentDC();
  SERIAL.printf("current DC: %f \n", currentDC);

  // compare to defined min value
  if (currentAC >= CURRENT_MIN_VALUE && !pumpeAktiv) {
    pumpeAktiv = true;
    sendAmpsToDB();
  }
  else if (currentAC <= CURRENT_MIN_VALUE && pumpeAktiv) {
    pumpeAktiv = false;
  }

  lastACTimeStamp = time_now;
}

if (checkTimer(time_now, lastCalTimeStamp, TIMER_INTERVAL_CALIBRATE) && !pumpeAktiv) {
  // calibrate the sensor after defined time. The calibrated value will only saved but nut used.
  lastCalTimeStamp=time_now;
  calValue = sensor.calibrate();
} 

 ArduinoOTA.handle();
 SERIAL.handle();
 yield();

}

/* 
 * retrieves environment values (temperature, humidity and pressure)
 */
void readEnvValues() {
  bme.read(globalPressure, globalTemp, globalHumidity,tempUnit, presUnit);

  SERIAL.printf("Humidity: %f \n", globalHumidity);
  SERIAL.printf("Temperatur: %f \n", globalTemp);
  SERIAL.printf("Pressure: %f \n", globalPressure);
}

/*
 * execute a check if new data should be retrieved
 */
boolean checkTimer(unsigned long currentTimeStamp, unsigned long lastTimeStamp, unsigned long timer_interval) {
  if (currentTimeStamp - lastTimeStamp < timer_interval) {
    return false;
  }
  else {
    return true;
  }
}

/*
 * init the BME sensor 
 */
void initBME() {
  Wire.begin();
  while(!bme.begin()) 
  {
    SERIAL.println("Could not find BME280 sensor!");
    delay(1000);
  } 
  settings.presOSR = BME280::OSR_X1;
  settings.humOSR = BME280::OSR_X16;

  bme.setSettings(settings);

}

/* 
 * init Over the air update 
 */
void initArduinoOTA() {
  ArduinoOTA.setHostname(HOSTNAME);

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_SPIFFS
      type = "filesystem";
    }

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
ArduinoOTA.begin();
}

/*
 * read AC value 
 */
float readACValue() {
  float current = sensor.getCurrentAC();
  if (current < 0.2f) {
    return 0.0f;
  }
  else {
    return current;
  }
}

/*
 * collect values and send the environment data to database
 */
void sendEnvToDB() {
    String requestData;
    requestData = requestData + "&temperatur=" + globalTemp + "&feuchtigkeit=" + globalHumidity
                          + "&luftdruck=" + globalPressure + ""; 
    sendHttpRequest(DB_PHP_INSERT_ENV_SCRIPT, requestData);
}

/*
 * collect Amps value and send it to database
 */
void sendAmpsToDB() {
  String requestData = "&Amps=";
  requestData = requestData + currentAC;
  sendHttpRequest(DB_PHP_INSERT_ACTIVITY_SCRIPT,requestData);
  }

/*
 * generates HTTP request and call the PHP script on webspace
 */
void sendHttpRequest(char* url, String requestData ) {
  HTTPClient http;
  http.begin(url);

  // Specify content-type header
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  // Prepare your HTTP POST request data
  String httpRequestData = "api_key=" + DB_PHP_API_KEY + requestData;

  SERIAL.print("httpRequestData: ");
  SERIAL.println(httpRequestData);

  // Send HTTP POST request
  int httpResponseCode = http.POST(httpRequestData);

  // If you need an HTTP request with a content type: text/plain
  http.addHeader("Content-Type", "text/plain");

  if (httpResponseCode > 0)
  {
    SERIAL.print("HTTP Response code: ");
    SERIAL.println(httpResponseCode);
    SERIAL.println(http.getString());
  }
  else
  {
    SERIAL.print("Error code: ");
    SERIAL.println(httpResponseCode);
  }
  // Free resources
  http.end();
}
