//new 
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "secrets.h"
#include <ArduinoJson.h>
#include <Wire.h>
#include "MAX30100_PulseOximeter.h"

#define REPORTING_PERIOD_MS    1000

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;
float BPM, SpO2, fahrenheit;
PulseOximeter pox;
uint32_t tsLastReport = 0;
int outputpin= A0;
int bufferValue = 1;

// Find this awsEndpoint in the AWS Console: Manage - Things, choose your thing
// choose Interact, its the HTTPS Rest endpoint 
const char* awsEndpoint = AWS_IOT_ENDPOINT;


BearSSL::X509List client_crt(AWS_CERT_CRT);
BearSSL::PrivateKey client_key(AWS_CERT_PRIVATE);
BearSSL::X509List rootCert(AWS_CERT_CA);

WiFiClientSecure wiFiClient;
void msgReceived(char* topic, byte* payload, unsigned int len);
PubSubClient pubSubClient(awsEndpoint, 1883, msgReceived, wiFiClient); 

void onBeatDetected()
{
  Serial.println("Beat Detected!");
}

void setupPox()
{
  Serial.print("Initializing pulse oximeter..");

  if (!pox.begin()) {
    Serial.println("FAILED");
    for (;;);
  } else {
    Serial.println("SUCCESS");
    
    pox.setOnBeatDetectedCallback(onBeatDetected);
  }
  
   pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);

 }

 void printPox()
 {
  Serial.print("BPM: ");
    Serial.println(BPM);

    Serial.print("SpO2: ");
    Serial.print(SpO2);
    Serial.println("%");

    Serial.println("*********************************");
    Serial.println();
  }
 
void setup() {
  Serial.begin(115200); 
  pinMode(16, OUTPUT);
  Serial.println();
  Serial.println("ESP8266 AWS IoT Example");

  Serial.print("Connecting to "); 
  Serial.print(ssid);
  WiFi.begin(ssid, password);
  WiFi.waitForConnectResult();
  Serial.print(", WiFi connected, IP address: "); 
  Serial.println(WiFi.localIP());
  setupPox();
  // get current time, otherwise certificates are flagged as expired
  setCurrentTime();

  wiFiClient.setClientRSACert(&client_crt, &client_key);
  wiFiClient.setTrustAnchors(&rootCert);
 
}

unsigned long lastPublish;
int msgCount;

void loop() {

  pubSubCheckConnect();
  pox.update();
  BPM = pox.getHeartRate();
  SpO2 = pox.getSpO2();
  fahrenheit = temperature();
  
  if (millis() - lastPublish > 1000) {
    StaticJsonDocument<200> doc;
    printPox();
    doc["BPM"] = BPM;
    doc["SpO2"] = SpO2;
    doc["Temprature"] = fahrenheit;  
    char jsonBuffer[512];
    serializeJson(doc, jsonBuffer); // print to client

    pubSubClient.publish("outTopic", jsonBuffer);
    lastPublish = millis();
  }
}

void msgReceived(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received on ");
  Serial.print(topic); 
  Serial.print(": ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void pubSubCheckConnect() {
  if ( ! pubSubClient.connected()) {
    Serial.print("PubSubClient connecting to: "); 
    Serial.print(awsEndpoint);
    while ( ! pubSubClient.connected()) {
      Serial.print(".");
      pubSubClient.connect(THINGNAME);
    }
    Serial.println(" connected");
    //pubSubClient.subscribe("inTopic");
  }
  //pubSubClient.loop();
}

float temperature()
{
  int analogValue = analogRead(outputpin);
  float millivolts = [(analogValue/1024.0) * 3300]-bufferValue; 
  float celsius = millivolts/10;
  
  fahrenheit = ((celsius * 9)/5 + 32);
  Serial.print(" in Farenheit=   ");
  Serial.println(fahrenheit);
  return fahrenheit;
  }

void setCurrentTime() {
  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");

  Serial.print("Waiting for NTP time sync: ");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(10);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: "); 
  Serial.print(asctime(&timeinfo));
}
