/* Flow Meter - Pump Relay - Restful App
 * connect to: /espPi
 *
 * running on: theta [192.168.1.207] - Water Pump
 * 
 * version 1.0.1
 * 
 * 1.0.1 - 5.10.2020 - stripped flow meter code, added rest function to setup
 * 1.0.0 - 25.1.2020 - initial version
 * 
 */
 
// Import required libraries
#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include <aREST.h>

// Create aREST instance
aREST rest = aREST();

// WiFi parameters
const char* ssid = "ZetaReticula";
const char* password = "0488425378";

#define LISTEN_PORT  80

// Create an instance of the server
WiFiServer server(LISTEN_PORT);

// Variables to be exposed to the API
char* location = "Water Pump";


// Declare functions to be exposed to the API
int relayControl(String command);


void setup() {
  // Serial Comunication init
  Serial.begin(115200);
  delay(10);

    // Init variables and expose them to REST API

    rest.variable("location", &location);

    // Give name and ID to device
    rest.set_id("207");
    rest.set_name("theta207");

    // Function to be exposed
    rest.function("On",relayControl);
  
    // Connect to WiFi
    WiFi.begin(ssid, password);
    IPAddress ip(192, 168, 1, 207); //set static ip
    IPAddress gateway(192, 168, 1, 1); //set getteway
    Serial.print(F("Setting static ip to : "));
    Serial.println(ip);
    IPAddress subnet(255, 255, 255, 0);//set subnet
    WiFi.config(ip, gateway, subnet);
     
    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
    Serial.println("");
    Serial.println("WiFi is connected");
    server.begin();
    Serial.println("Server started");
    Serial.println(WiFi.localIP());
    Serial.println("Location: ");
    Serial.println(location);


}

void loop() {
 

     // Handle REST calls
    WiFiClient client = server.available();
    if (!client) {
      return;
    }
    while(!client.available()){
      delay(1);
    }
    rest.handle(client);
    Serial.println("web request has been made");
    
}

// Custom function accessible by the API
int relayControl(String command) {

  // Get state from command
  int state = command.toInt();

  digitalWrite(6,state);
  return 1;
}
