# sfuit-esp8266

This Repository is used for connecting Esp8266 to AWS IOT Core. Repository consists of two files
1) PubSubTest.ino 
2) Secrets.h

***Before getting started, you need to set up your Arduino IDE with ESP8266 environment.*** 

In **PubSubTest.ino** file, the procedure for connecting your ESP8266 to AWS IOT Core is mentioned. 

**Methods**
Apart from setup() and loop(), seven other methods in PubSubTest.ino file are implemented, they are:
1. onBeatDetected()
2. setupPox()
3. printPox()
4. msgReceived()
5. PubSubCheckConnect()
6. temperature()
7. setCurrentTime()

## WorkFlow:
To begin with, in setup() after the wifi connection is established, then we invoke setupPox() method for initializing [MAX30100 Sensor](https://datasheets.maximintegrated.com/en/ds/MAX30100.pdf). Then we invoke setCurrentTime() method to configure the time from NTP org as shown :
`configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");`
This line make sures that the certificates arent flagged as expired. AWS IOT CORE certificates for a particular THING are verified once the connection is established.
This marks the end of setUp() method.
Coming to loop() method, we make sure our PubSubClient is connected to our application using PubSubCheckConnect() method.
If beats are detected by MAX30100 sensor, then onBeatDetected() is immediately invoked and hence we could say that MAX30100 is working properly. 
Since we are using PubSubClient which runs on MQTT protocol, we must publish a topic in ESP8266 which is subscribed by our AWS IOT Core MQTT Console.
`pubSubClient.connect(THINGNAME);`
This statement after being triggered successfully sends the published topic and value paired to it to AWS IOT Core MQTT Console where topic is being Subscribed. 
For checking whether correct data being transferred, we use msgReceive() method.
After all these successfull invocations of methods, we finally get the raw data of Heart Rate in BPM and Oxygen levels in percentage to which appropriate algorithms are applied for further processing.
Finally, temperature() method is used for calculating the body temperature, which is read by [LM35 sensor](https://www.ti.com/lit/ds/symlink/lm35.pdf).

These values after being processed are sent to AWS IOT CORE in JSON format.

**Secrets.h file** 

This file consists of three Certificates provided by AWS IOT CORE which are unique for every THING. 
This helps in device authentication.

## CITATIONS:

- ESP8266WFIF.h by Arduino
- PubSubClient.h - A client library for MQTT Messaging devoleped by Nick O'Leary
- ArduinoJson.h - A simple and efficient JSON Library for embedded C++
- Wire.h 
- MAX30100_PulseOximeter.h - Supports the MAX30100 Pulse Oximetry IC developed by Connor Huffine

***All these libraries are installed from Arduino IDE***
