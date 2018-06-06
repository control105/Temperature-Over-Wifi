/*
 ESP8266 WiFi module Temperature monitor

  A project to monitor temperature in a local network using ESP8266 WiFi module

  Connecting Module:
  pin9 -> WiFi-module-Tx
  pin10 -> WiFi-module-Rx
  pin4 -> DHT11 digital output pin


  Dependency:
  WiFiEsp library(https://github.com/bportaluri/WiFiEsp/)
  DHT library(https://learn.adafruit.com/dht)(https://github.com/adafruit/Adafruit_Sensor)
*/

#include "WiFiEsp.h"
#include <SoftwareSerial.h>
#include "DHT.h"



//Create software serial object for ESP8366 WiFi module
SoftwareSerial ESP8266(2, 3); //RX, TX

char ssid[] = "Temperature Monitor";         //Network SSID (name)
char pass[] = "12345678";        //Network password

String cData;

boolean FAIL_8266 = false;

int status = WL_IDLE_STATUS;     // the Wifi radio's status
int reqCounter = 0;                // number of requests received

WiFiEspServer esp8266Server(80);

// use a ring buffer to increase speed and reduce memory allocation
RingBuffer buf(8);

unsigned short conLed = 5;
unsigned short conFlag = 0;

void setup()
{
  pinMode(conLed, OUTPUT);
  Serial.begin(9600);   // initialize serial for debugging
  ESP8266.begin(9600);    // initialize serial for ESP module
  WiFi.init(&ESP8266);    // initialize ESP module

  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    while (true); // don't continue
  }

  Serial.print("Attempting to start AP ");
  Serial.println(ssid);

  
  // start access point
  status = WiFi.beginAP(ssid, 10, pass, ENC_TYPE_WPA2_PSK);

  Serial.println("Client Connected IP Address: ");
  WifiStatus();
  
  // start the web server on port 80
  esp8266Server.begin();
  Serial.println("Server started");
}


void loop()
{
  WiFiEspClient client = esp8266Server.available();  // listen for incoming clients
  if (client) {                               // if you get a client,
    conFlag = 1;
    Serial.println("New client");             // print a message out the serial port
    buf.init();                               // initialize the circular buffer
    while (client.connected()) {              // loop while the client's connected
      if (client.available()) {               // if there's bytes to read from the client,
        char c = client.read();               // read a byte, then
        buf.push(c);                          // push it to the ring buffer
        cData += c;

        // you got two newline characters in a row
        // that's the end of the HTTP request, so send a response
        if (buf.endsWith("\r\n\r\n")) {
          Serial.print("Client Message: ");
          Serial.println(cData);
          Serial.print("Client Message indexof : ?data=0");
          Serial.println(cData.indexOf("?data=0"));
          if(cData.indexOf("?data=0") !=  -1){
            sendHttpResponse(client);
          }
          break;          
        }
      }
    }
    
    // give the client time to receive the data
    delay(50);

    // close the connection
    client.stop();
    Serial.println("Client disconnected");
  }
  else{
        conFlag = 0;
  }
  if(conFlag){
    digitalWrite(conLed, HIGH);
  }
  else{
    digitalWrite(conLed, LOW);
  }
}

void sendHttpResponse(WiFiEspClient client)
{
  client.print(
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html\r\n"
    "Connection: close\r\n"  // the connection will be closed after completion of the response
    "Refresh: 10\r\n"        // refresh the page automatically every 20 sec
    "\r\n");
  client.print("<!DOCTYPE HTML>\r\n");
  client.print("<html>\r\n");
  client.print("RequestsN: ");
  client.print(++reqCounter);
  client.print("<br>\r\n");
  client.print("The sensor: ");
  client.print(getSensorsData());
  client.print("<br>\r\n");
  client.print("</html>\r\n");
}

void WifiStatus()
{
  // print your WiFi module IP address
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
}



