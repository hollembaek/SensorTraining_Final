// Basic demos for PCT2075 and H3LIS331 Merged + WiFi
// https://github.com/adafruit/Adafruit_PCT2075/blob/master/examples/pct2075_test/pct2075_test.ino
// https://github.com/adafruit/Adafruit_LIS331/blob/8088c93b8252b42cc68e098123d0426d784105b0/examples/h3lis331_accel_demo/h3lis331_accel_demo.ino

#include <Wire.h>
#include <SPI.h>
#include <WiFi.h>
#include <Adafruit_H3LIS331.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_PCT2075.h>

// Used for software SPI
#define H3LIS331_SCK 13
#define H3LIS331_MISO 12
#define H3LIS331_MOSI 11
// Used for hardware & software SPI
#define H3LIS331_CS 10

//Sensors
Adafruit_H3LIS331 lis = Adafruit_H3LIS331();
Adafruit_PCT2075 PCT2075;

// WiFi Connection
const char* ssid     = "yourSSID";
const char* password = "yourWLANpwd";

void setup(void) {
  Serial.begin(115200);
  while (!Serial) delay(10);     // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("Sensor Connection Test!");

//  if (!lis.begin_SPI(H3LIS331_CS)) {
//  if (!lis.begin_SPI(H3LIS331_CS, H3LIS331_SCK, H3LIS331_MISO, H3LIS331_MOSI)) {
//Check for Accelerometer
 if (! lis.begin_I2C()) {   // change this to 0x19 for alternative i2c address
    Serial.println("Couldnt start");
    while (1) yield();
  }
  Serial.println("H3LIS331 found!");
  
//Check for Thermometer
 if (!PCT2075.begin()) {
    Serial.println("Couldn't find PCT2075 chip");
    while (1);
  }
  
  Serial.println("Found PCT2075 chip");

  // lis.setRange(H3LIS331_RANGE_100_G);   // 100, 200, or 400 G!
  Serial.print("LIS Range set to: ");
  switch (lis.getRange()) {
    case H3LIS331_RANGE_100_G: Serial.println("100 g"); break;
    case H3LIS331_RANGE_200_G: Serial.println("200 g"); break;
    case H3LIS331_RANGE_400_G: Serial.println("400 g"); break;
  }
  // lis.setDataRate(LIS331_DATARATE_1000_HZ);
  Serial.print("LIS Data rate set to: ");
  switch (lis.getDataRate()) {

    case LIS331_DATARATE_POWERDOWN: Serial.println("Powered Down"); break;
    case LIS331_DATARATE_50_HZ: Serial.println("50 Hz"); break;
    case LIS331_DATARATE_100_HZ: Serial.println("100 Hz"); break;
    case LIS331_DATARATE_400_HZ: Serial.println("400 Hz"); break;
    case LIS331_DATARATE_1000_HZ: Serial.println("1000 Hz"); break;
    case LIS331_DATARATE_LOWPOWER_0_5_HZ: Serial.println("0.5 Hz Low Power"); break;
    case LIS331_DATARATE_LOWPOWER_1_HZ: Serial.println("1 Hz Low Power"); break;
    case LIS331_DATARATE_LOWPOWER_2_HZ: Serial.println("2 Hz Low Power"); break;
    case LIS331_DATARATE_LOWPOWER_5_HZ: Serial.println("5 Hz Low Power"); break;
    case LIS331_DATARATE_LOWPOWER_10_HZ: Serial.println("10 Hz Low Power"); break;

  }
  
// WiFi setup - Note that the connection is not used in this sketch.
// If you do not add connected services, you can comment out this section and run without wifi.
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
}

void loop() {
  /* Get a new sensor event, normalized */
  sensors_event_t event;
  lis.getEvent(&event);

  /* Display the results (acceleration is measured in m/s^2) */
  Serial.print("\t\tX: "); Serial.print(event.acceleration.x);
  Serial.print(" \tY: "); Serial.print(event.acceleration.y);
  Serial.print(" \tZ: "); Serial.print(event.acceleration.z);
  Serial.println(" m/s^2 ");

  /* Alternately, given the range of the H3LIS331, display the results measured in g */
  // Serial.print("\t\tX:"); Serial.print(event.acceleration.x / SENSORS_GRAVITY_STANDARD);
  // Serial.print(" \tY: "); Serial.print(event.acceleration.y / SENSORS_GRAVITY_STANDARD);
  // Serial.print(" \tZ: "); Serial.print(event.acceleration.z / SENSORS_GRAVITY_STANDARD);
  // Serial.println(" g");
  
  // Temperature Reading
  Serial.print("Temperature: "); Serial.print(PCT2075.getTemperature());Serial.println(" C");

  delay(1000);
}
