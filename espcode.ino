#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>

// Watson IoT connection details
#define MQTT_HOST "24uftm.messaging.internetofthings.ibmcloud.com"
#define MQTT_PORT 1883
#define MQTT_DEVICEID "d:24uftm:ESP8266:dev01"
#define MQTT_USER "use-token-auth"
#define MQTT_TOKEN "123456789"
#define MQTT_TOPIC "iot-2/evt/status/fmt/json"
#define MQTT_TOPIC_CMD "iot-2/cmd/display/fmt/json"

// Add WiFi connection information
char ssid[] = "Shepy";  //  your network SSID (name)
char pass[] = "shepy78370";  // your network password

// MQTT objects
void callback(char* topic, byte* payload, unsigned int length);
WiFiClient wifiClient;
PubSubClient mqtt(MQTT_HOST, MQTT_PORT, callback, wifiClient);

// variables to hold data
StaticJsonBuffer<200> jsonBuffer;
JsonObject& payload = jsonBuffer.createObject();
JsonObject& status = payload.createNestedObject("d");
static char msg[100];
const byte numChars = 90;
char temp[numChars];
char humidity[numChars];
char lumi[numChars];
char pressure[numChars];
char altitude[numChars];
char uv[numChars];
boolean newData = false;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println();
  Serial.println("ESP8266 Sensor Application");

  // Start WiFi connection
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi Connected");

  if (mqtt.connect(MQTT_DEVICEID, MQTT_USER, MQTT_TOKEN)) {
    Serial.println("MQTT Connected");
    mqtt.subscribe(MQTT_TOPIC_CMD);

  } else {
    Serial.println("MQTT Failed to connect!");
    ESP.reset();
  }
}

void loop() {
  mqtt.loop();
  while (!mqtt.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (mqtt.connect(MQTT_DEVICEID, MQTT_USER, MQTT_TOKEN)) {
      Serial.println("MQTT Connected");
      mqtt.subscribe(MQTT_TOPIC_CMD);
      mqtt.loop();
    } else {
      Serial.println("MQTT Failed to connect!");
      delay(5000);
    }
  }
  recovWithStartEndMarkers();
  showNewData();
}

void recovWithStartEndMarkers() {
  static boolean humiRecvInProgress = false;
  static boolean lumiRecvInProgress = false;
  static boolean presRecvInProgress = false;
  static boolean altRecvInProgress = false;
  static boolean tempRecvInProgress = false;
  static boolean uvRecvInProgress = false;

  static byte ndx = 0;
  char humiStartMarker = 'a';
  char humiEndMarker = 'b';
  char lumiStartMarker = 'e';
  char lumiEndMarker = 'f';
  char presStartMarker = 'g';
  char presEndMarker = 'h';
  char altStartMarker = 'i';
  char altEndMarker = 'j';
  char tempStartMarker = 'k';
  char tempEndMarker = 'l';
  char uvStartMarker = 'q';
  char uvEndMarker = 'r';
  char rc;

  while (Serial.available() > 0 && newData == false) {
    rc = Serial.read();

    // -- humidity --
    if (humiRecvInProgress == true) {
      if (rc != humiEndMarker) {
        humidity[ndx] = rc;
        ndx++;
        if (ndx >= numChars) {
          ndx = numChars - 1;
        }
      }
      else {
        humidity[ndx] = '\0'; // terminate the string
        humiRecvInProgress = false;
        ndx = 0;
      }
    }
    else if (rc == humiStartMarker) {
      humiRecvInProgress = true;
    }

    // -- light --
    if (lumiRecvInProgress == true) {
      if (rc != lumiEndMarker) {
        lumi[ndx] = rc;
        ndx++;
        if (ndx >= numChars) {
          ndx = numChars - 1;
        }
      }
      else {
        lumi[ndx] = '\0'; // terminate the string
        lumiRecvInProgress = false;
        ndx = 0;
      }
    }
    else if (rc == lumiStartMarker) {
      lumiRecvInProgress = true;
    }

    // -- pressure --
    if (presRecvInProgress == true) {
      if (rc != presEndMarker) {
        pressure[ndx] = rc;
        ndx++;
        if (ndx >= numChars) {
          ndx = numChars - 1;
        }
      }
      else {
        pressure[ndx] = '\0'; // terminate the string
        presRecvInProgress = false;
        ndx = 0;
      }
    }
    else if (rc == presStartMarker) {
      presRecvInProgress = true;
    }

    // -- altitude --
    if (altRecvInProgress == true) {
      if (rc != altEndMarker) {
        altitude[ndx] = rc;
        ndx++;
        if (ndx >= numChars) {
          ndx = numChars - 1;
        }
      }
      else {
        altitude[ndx] = '\0'; // terminate the string
        altRecvInProgress = false;
        ndx = 0;
      }
    }
    else if (rc == altStartMarker) {
      altRecvInProgress = true;
    }

    // -- temperature --
    if (tempRecvInProgress == true) {
      if (rc != tempEndMarker) {
        temp[ndx] = rc;
        ndx++;
        if (ndx >= numChars) {
          ndx = numChars - 1;
        }
      }
      else {
        temp[ndx] = '\0'; // terminate the string
        tempRecvInProgress = false;
        ndx = 0;
      }
    }
    else if (rc == tempStartMarker) {
      tempRecvInProgress = true;
    }

    // -- uv --
    if (uvRecvInProgress == true) {
      if (rc != uvEndMarker) {
        uv[ndx] = rc;
        ndx++;
        if (ndx >= numChars) {
          ndx = numChars - 1;
        }
      }
      else {
        uv[ndx] = '\0'; // terminate the string
        uvRecvInProgress = false;
        ndx = 0;
        newData = true;
      }
    }
    else if (rc == uvStartMarker) {
      uvRecvInProgress = true;
    }
  }
}

void showNewData() {
  if (newData == true) { // data has been recovered
    float humi = atof(humidity); // conversion from string to float
    float lumine = atof(lumi);
    float pres = atof(pressure) / 100;
    float alt = atof(altitude);
    float temperature = atof(temp);
    float uvindex = atof(uv);
    Serial.print("Humidity: ");
    Serial.print(humi);
    Serial.print(" % ");
    Serial.print("Temperature1: ");
    Serial.print("Light intensity: ");
    Serial.print(lumine);
    Serial.print(" Ca ");
    Serial.print("Pressure: ");
    Serial.print(pres);
    Serial.print(" Pa ");
    Serial.print("Altitude: ");
    Serial.print(alt);
    Serial.print(" m ");
    Serial.print("Temperature : ");
    Serial.print(temperature);
    Serial.print(" C ");
    Serial.print("UV index: ");
    Serial.println(uvindex);
    newData = false;

    // Send data to Watson IoT Platform
    status["humidity"] = humi;
    status["luminance"] = lumine;
    status["pressure"] = pres;
    status["altitude"] = alt;
    status["temp"] = temperature;
    status["uv"] = uvindex;
    payload.printTo(msg, 1000);
    Serial.println(msg);
    if (!mqtt.publish(MQTT_TOPIC, msg)) {
      Serial.println("MQTT Publish failed");
    }
  }

  // Pause - but keep polling MQTT for incoming messages
  for (int i = 0; i < 10; i++) {
    mqtt.loop();
    delay(100);
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.println("] ");
}
