#define DEBUG_ENABLED true
#define FLASH_BUILTIN_LED true
#define READING_DELAY 8000 // reading delay in milliseconds

// Wifi: SSID and password
const char* WIFI_SSID = "";
const char* WIFI_PASSWORD = "";

// MQTT: ID, server IP, port, username and password
const char* MQTT_CLIENT_ID = ""; // should be unique
const char* MQTT_SERVER_IP = ""; // IP of the MQTT broker
const uint16_t MQTT_SERVER_PORT = 1883; // default MQTT port
const char* MQTT_USERNAME = "";
const char* MQTT_PASSWORD = "";
const char* MQTT_CAR_CONTROL_TOPIC = "car/control";

// Note for directions:
// pitch < 0 -> go down
// pitch > 0 -> go up
// roll < 0 -> right
// roll > 0 -> left
