/*---------------------------------------------------------------------------------------------

  Open Sound Control (OSC) library for the ESP8266/ESP32

  Example for sending messages from the ESP8266/ESP32 with a distance sensor
  to a remote computer via osc to the address "/distance".

  This example code is in the public domain.

  --------------------------------------------------------------------------------------------- */
#if defined(ESP8266)
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif
#include <WiFiUdp.h>
#include <OSCMessage.h>


char ssid[] = "57M1";                   // your network SSID (name)
char pass[] = "Snare-Pancake!";         // your network password


WiFiUDP Udp;                                // A UDP instance to let us send and receive packets over UDP
const IPAddress outIp(192, 168, 188, 114);     // remote IP of your computer
const unsigned int outPort = 8000;          // remote port to receive OSC
const unsigned int localPort = 9000;        // local port to listen for OSC packets (actually not used for sending)

/*------------------------
   Put here below the arduino variables
*/

const unsigned int TRIG_PIN = D0;
const unsigned int ECHO_PIN = D1;

//define sound velocity in cm/uS
#define SOUND_VELOCITY 0.034


void setup() {
  Serial.begin(115200);

  pinMode(LED_BUILTIN, OUTPUT);  // Initialize the LED_BUILTIN pin as an output

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Starting UDP");
  Udp.begin(localPort);
  Serial.print("Local port: ");
#ifdef ESP32
  Serial.println(localPort);
#else
  Serial.println(Udp.localPort());
#endif
  /*-----
     Put here below the arduino set up
  */
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

}

void loop() {

  /*-------
     Put bellow here the sensor code
  */
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  const unsigned long duration = pulseIn(ECHO_PIN, HIGH);
  float distance = duration * SOUND_VELOCITY / 2;
  float distanceToSend;
  if (duration == 0) {
    //Serial.println(300);
    //Serial.println("Warning: no pulse from sensor");
    //Serial.println("NaN");
    distanceToSend = 1000;
  }
  else {
    // Serial.print("distance to nearest object:");
    Serial.println(distance);
    //Serial.println(" cm");
    distanceToSend = distance;
  }
  

  /*---------
     Put here the osc message
  */
  OSCMessage msg("/distance");
  msg.add(distanceToSend);
  Udp.beginPacket(outIp, outPort);
  msg.send(Udp);
  Udp.endPacket();
  msg.empty();

  //Serial.println("breakpoint");
  digitalWrite(LED_BUILTIN, 1);
  delay(500);
  digitalWrite(LED_BUILTIN, 0);

  delay(10);
}
