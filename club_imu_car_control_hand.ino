#include <WiFi.h> 
#include <PubSubClient.h> 
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include "config.h"

// MQTT
WiFiClient espClient;
PubSubClient client(espClient);

// IMU sensor
Adafruit_MPU6050 mpu;

unsigned long lastPublish = 0;
const unsigned long publishInterval = 1000;

void setup() {
  Serial.begin(9600); 

  // Setup wifi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD); 

  while (WiFi.status() != WL_CONNECTED) {
    delay(500); 
    Serial.println(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Setup MQTT server
  client.setServer(MQTT_SERVER_IP, MQTT_SERVER_PORT); 
  client.setCallback(callback); 

  while (!client.connected()) {
    if (client.connect(MQTT_CLIENT_ID, MQTT_USERNAME, MQTT_PASSWORD)) {
        Serial.println("Public EMQX MQTT broker connected");
    } else {
        Serial.println("Connection failed with state ");
        Serial.print(client.state());
        delay(2000);
    }
  }

  client.publish(MQTT_CAR_CONTROL_TOPIC, "ESP32 Car - Connected");
  client.subscribe(MQTT_CAR_CONTROL_TOPIC);

  // Setup IMU sensor
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
  case MPU6050_RANGE_2_G:
    Serial.println("+-2G");
    break;
  case MPU6050_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case MPU6050_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case MPU6050_RANGE_16_G:
    Serial.println("+-16G");
    break;
  }
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  Serial.print("Gyro range set to: ");
  switch (mpu.getGyroRange()) {
  case MPU6050_RANGE_250_DEG:
    Serial.println("+- 250 deg/s");
    break;
  case MPU6050_RANGE_500_DEG:
    Serial.println("+- 500 deg/s");
    break;
  case MPU6050_RANGE_1000_DEG:
    Serial.println("+- 1000 deg/s");
    break;
  case MPU6050_RANGE_2000_DEG:
    Serial.println("+- 2000 deg/s");
    break;
  }

  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
  case MPU6050_BAND_260_HZ:
    Serial.println("260 Hz");
    break;
  case MPU6050_BAND_184_HZ:
    Serial.println("184 Hz");
    break;
  case MPU6050_BAND_94_HZ:
    Serial.println("94 Hz");
    break;
  case MPU6050_BAND_44_HZ:
    Serial.println("44 Hz");
    break;
  case MPU6050_BAND_21_HZ:
    Serial.println("21 Hz");
    break;
  case MPU6050_BAND_10_HZ:
    Serial.println("10 Hz");
    break;
  case MPU6050_BAND_5_HZ:
    Serial.println("5 Hz");
    break;
  }

  Serial.println("");
  delay(100);

}

void loop() {
  client.loop(); 

  unsigned long currentMillis = millis();
  if (currentMillis - lastPublish >= publishInterval) {
    lastPublish = currentMillis;

    sensors_event_t ac, gy, tem;
    mpu.getEvent(&ac, &gy, &tem);

    int roll = round(constrain(ac.acceleration.x, -10, 10)); 
    int pitch = round(constrain(ac.acceleration.y, -10, 10));

    char msg[10];
    sprintf(msg, "%d;%d", roll, pitch);

    client.publish(MQTT_CAR_CONTROL_TOPIC, msg);
  }

}

void callback(char *topic, byte *payload, unsigned int length) {
    Serial.println("Ping: Hand");
    Serial.print("Topic: ");
    Serial.println(topic);
    Serial.print("Message:");
    for (int i = 0; i < length; i++) {
        Serial.print((char) payload[i]);
    }
    Serial.println();
    Serial.println("-----------------------");
}



