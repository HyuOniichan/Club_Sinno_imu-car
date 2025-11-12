#include <WiFi.h>
#include <PubSubClient.h>
#include "config.h"

// TB6612FNG
#define PWMA 33
#define AIN1 26
#define AIN2 25

#define PWMB 13
#define BIN1 14
#define BIN2 12

#define STBY 27

// MQTT Server
WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastPublish = 0;
const unsigned long publishInterval = 1000;

int roll = 0;
int pitch = 0;

int right = 0;
int left = 0;
int velocity = 100;

int lastPitch = 0;
int rollVariance = 3;
int velocityDecay = 20;
int stopDecay = 50;

void setup() {
  Serial.begin(115200);

  // Wifi setup
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // MQTT setup
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

  // Motor driver setup
  pinMode(PWMA, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(PWMB, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(STBY, OUTPUT);

  digitalWrite(STBY, HIGH);
}

void loop() {
  client.loop();

  // Control motors
  digitalWrite(AIN1, 1);
  digitalWrite(AIN2, 0);
  digitalWrite(BIN1, 1);
  digitalWrite(BIN2, 0);

  analogWrite(PWMA, velocity * right);
  analogWrite(PWMB, velocity * left);
}

void rotateMotor(int roll, int pitch) {
  Serial.print("rotateMotor(");
  Serial.print(roll);
  Serial.print(", ");
  Serial.print(pitch);
  Serial.println(")");
  Serial.println("Values:");
  Serial.print("Right: ");
  Serial.print(right);
  Serial.print(", ");
  Serial.print("Left: ");
  Serial.print(left);
  Serial.print(", ");
  Serial.print("Velocity: ");
  Serial.println(velocity);

  if (pitch <= 1) {
    // Start the motor from max velocity
    if (lastPitch > 1) {
      velocity = 200;
    }
  } else {
    right = 1;
    left = 1;
    if (velocity - stopDecay >= 0) {
      velocity -= stopDecay;
    }
  }

  lastPitch = pitch;

  if (velocity - velocityDecay >= 100) {
    velocity -= velocityDecay;
  }

  if (roll < -rollVariance) {
    right = 1;
    left = 0;
  } else if (roll > rollVariance) {
    right = 0;
    left = 1;
  } else right = left = 1;
}

void parseRollPitch(const char *msg, int &roll, int &pitch) {
  char buffer[32];
  strncpy(buffer, msg, sizeof(buffer));
  buffer[sizeof(buffer) - 1] = '\0';

  char *token = strtok(buffer, ";");
  if (token != NULL) {
    roll = atoi(token);
    token = strtok(NULL, ";");
    if (token != NULL) {
      pitch = atoi(token);
    }
  }

  rotateMotor(roll, pitch);
}


void callback(char *topic, byte *payload, unsigned int length) {
  if (strcmp(topic, MQTT_CAR_CONTROL_TOPIC) == 0) {
    char msg[10];
    memcpy(msg, payload, length);
    msg[length] = '\0';

    int roll, pitch;
    parseRollPitch(msg, roll, pitch);
  }

  Serial.println("Ping: Car");
  Serial.print("Topic: ");
  Serial.println(topic);
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  Serial.println("-----------------------");
}
