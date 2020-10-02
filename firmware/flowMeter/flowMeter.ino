/* Flow Meter - Pump Relay - Restful App
 * connect to: /espPi
 *
 * running on: theta [192.168.1.207] - Water Pump
 * 
 * version 1.0.0
 * 
 * ver 1.0.0 - 25.1.2020
 */
 
// Import required libraries
#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include <aREST.h>

// Create aREST instance
aREST rest = aREST();

// WiFi parameters
const char* ssid = "proxima";
const char* password = "torch3helene";

#define LISTEN_PORT  80
#define PULSE_PIN D2  //gpio4

// Create an instance of the server
WiFiServer server(LISTEN_PORT);

// Variables to be exposed to the API
char* location = "Water Pump";
volatile long pulseCount=0;
float calibrationFactor = 4.5;
float flowRate;
unsigned int flowMilliLitres;
unsigned long totalMilliLitres;
float totalLitres;
unsigned long oldTime;

// Declare functions to be exposed to the API
int relayControl(String command);

void ICACHE_RAM_ATTR pulseCounter()
{
  pulseCount++;
}

void setup() {
  // Serial Comunication init
  Serial.begin(115200);
  delay(10);

  // Initialization
  pinMode(PULSE_PIN, INPUT);
  digitalWrite(PULSE_PIN, HIGH);

  pulseCount        = 0;
  flowRate          = 0.0;
  flowMilliLitres   = 0;
  totalMilliLitres  = 0;
  oldTime           = 0; 

  // The Hall-effect sensor is connected to pin 2 which uses interrupt 0.
  // Configured to trigger on a FALLING state change (transition from HIGH
  // state to LOW state)
  attachInterrupt(PULSE_PIN, pulseCounter, FALLING);


    // Init variables and expose them to REST API
    rest.variable("flowRate", &flowRate);
    rest.variable("totalLitres", &totalLitres);
    rest.variable("location", &location);

    // Give name and ID to device
    rest.set_id("207");
    rest.set_name("theta207");
  
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
    // Read flow meter
    //flowRate = dataRead(D2);
  

// ********
   if((millis() - oldTime) > 1000)    // Only process counters once per second
  {
    detachInterrupt(PULSE_PIN);
    flowRate = ((1000.0 / (millis() - oldTime)) * pulseCount) / calibrationFactor;
    oldTime = millis();
    flowMilliLitres = (flowRate / 60) * 1000;
    totalMilliLitres += flowMilliLitres;     
    totalLitres = totalMilliLitres * 0.001;
    unsigned int frac;   
    Serial.print("flowrate: ");
    Serial.print(int(flowRate));  // Print the integer part of the variable
    Serial.print(".");             // Print the decimal point
    frac = (flowRate - int(flowRate)) * 10;
    Serial.print(frac, DEC) ;      // Print the fractional part of the variable
    Serial.print("L/min");
    Serial.print("  Current Liquid Flowing: ");             // Output separator
    Serial.print(flowMilliLitres);
    Serial.print("mL/Sec");
    Serial.print("  Output Liquid Quantity: ");             // Output separator
    Serial.print(totalLitres);
    Serial.println("L");
    //Serial.print(totalMilliLitres);
    //Serial.println("mL");

    pulseCount = 0;

    attachInterrupt(PULSE_PIN, pulseCounter, FALLING);
  }
// **********

    

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
