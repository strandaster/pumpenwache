#define SSID  "<YOUR_WLAN_SSID>"
#define WLAN_KEY "<YOUR_WLAN_KEY>"

#define HOSTNAME "<YOUR_HOSTNAME>"

#ifndef min
#define min(a,b) ((a)<(b)?(a):(b))
#endif
#ifndef max
#define max(a,b) ((a)>(b)?(a):(b))
#endif

void readEnvValues();
void initBME();
void initArduinoOTA();
void initRemoteDebug();
float readACValue();
void sendEnvToDB();
void sendAmpsToDB();
void sendHttpRequest(char* url, String requestData );

boolean checkTimer(unsigned long, unsigned long, unsigned long);

BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);  
BME280::PresUnit presUnit(BME280::PresUnit_hPa);

#define ACS712PIN A0
#define ACS_CALIBRATION_VALUE 785 //muss noch mittels calibrate ermittelt werden und dann per setZero gesetzt werden.
#define VOLTAGE_REFERENCE_VALUE 4.4

#define TIMER_INTERVAL_UPDATE_ENV  900000 //15 min
#define TIMER_INTERVAL_AC 15000 // 15s
#define TIMER_INTERVAL_CALIBRATE 3600000 //60 min 
#define CURRENT_MIN_VALUE 1.8

#define DB_PHP_INSERT_ENV_SCRIPT "http://<YOUR_WEBSPACE>/pumpenwache-post-data.php"
#define DB_PHP_INSERT_ACTIVITY_SCRIPT "http://<YOUR_WEBSPACE>/pumpenwache-post-activity.php"
String DB_PHP_API_KEY = "<YOUR_API_KEY>";