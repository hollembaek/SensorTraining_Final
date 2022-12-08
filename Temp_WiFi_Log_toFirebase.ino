/**
   Based on ESP32 + DHT22 Example for Wokwi
   https://wokwi.com/arduino/projects/322410731508073042
   ...and ESP32 Data Logging for firebase
   https://randomnerdtutorials.com/esp32-data-logging-firebase-realtime-database/
   ...modified to get readings from a PCT2075 digital thermometer
*/

#include "WiFi.h"
#include "FirebaseESP32.h"
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#include "time.h"
#include "Adafruit_Sensor.h"
#include "Adafruit_PCT2075.h"

// Insert Firebase project API Key
#define API_KEY "your_api_key"

// Insert RTDB URLefine the RTDB URL 
#define DATABASE_URL "https://your_app_url" 

// Insert Authorized Email and Corresponding Password
// You make these in the firebase console- does not need to be a "real" account
// Several Authentication mechansism are supported but this is easy to implement for demo purposes

#define USER_EMAIL "your_user_id"
#define USER_PASSWORD "your_user_pwd"

//Define Firebase Data object
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
FirebaseJson json;

//PCTSensor;
Adafruit_PCT2075 PCT2075;

// Variable to save USER UID
String uid;
// Database main path (to be updated in setup with the user UID)
String databasePath;
// Database child nodes - see Logging tutorial above for explanation of model
String tempPath = "/temperature";
String timePath = "/timestamp";

// Parent Node (to be updated in every loop)
String parentPath;

int count = 0;
bool signupOK = false;
int timestamp;
const char* ntpServer = "pool.ntp.org";

// Timer variables (send new readings every three minutes)
unsigned long sendDataPrevMillis = 0;
unsigned long timerDelay = 180000;

// Function that gets current epoch time
unsigned long getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    //Serial.println("Failed to obtain time");
    return(0);
  }
  time(&now);
  return now;
}

void setup() {
  Serial.begin(115200);
  //Check for Thermometer
  if (!PCT2075.begin()) {
    Serial.println("Couldn't find PCT2075 chip");
    while (1);
  }
  Serial.println("Found PCT2075 chip");
  
  // Assign the api key (required) 
  config.api_key = API_KEY;
  // Assign the RTDB URL (required)
  config.database_url = DATABASE_URL;
  // Assign the user sign in credentials
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  configTime(0, 0, ntpServer);
  
  Firebase.reconnectWiFi(true);
  fbdo.setResponseSize(4096);
  // Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  // Assign the maximum retry of token generation
  config.max_token_generation_retry = 5;

  // Update the WiFi connection info for your WLAN
  WiFi.begin("your_SSID", "your_WLAN_secret");
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println(" Connected!");
  Serial.println(WiFi.localIP());

    // Initialize the library with the Firebase authen and config
  Firebase.begin(&config, &auth);

  // Getting the user UID might take a few seconds
  Serial.println("Getting User UID");
  while ((auth.token.uid) == "") {
    Serial.print('.');
    delay(1000);
  }
  // Print user UID - Seeing this in the console indicates connection to Firebase was successful
  uid = auth.token.uid.c_str();
  Serial.print("User UID: ");
  Serial.println(uid);

  // Update database path
  databasePath = "/UsersData/" + uid + "/readings";

}

void loop() {
    /* Get a new sensor event, normalized */
  sensors_event_t event;

  // Send new readings to database
  if (Firebase.ready() && (millis() - sendDataPrevMillis > timerDelay || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();

    //Get current timestamp
    timestamp = getTime();
    Serial.print ("time: ");
    Serial.println (timestamp);

    parentPath= databasePath + "/" + String(timestamp);

    json.set(tempPath.c_str(), String(PCT2075.getTemperature()));
    json.set(timePath, String(timestamp));
    Serial.printf("Set json... %s\n", Firebase.RTDB.setJSON(&fbdo, parentPath.c_str(), &json) ? "ok" : fbdo.errorReason().c_str());
  }
}
