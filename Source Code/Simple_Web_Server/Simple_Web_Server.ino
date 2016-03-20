/* This code is intended for the purpose of displaying basic temperature 
 * information on a webpage.
 * This code builds on top of example code & libraries compiled
 * for easy use on: https://github.com/maanitmehra/arduino-workshop
 * 
 * Original Code Author: Maanit Mehra
 * Date: 20th March, 2015
 */

// Importing Required Libraries.
#include <Adafruit_CC3000.h>
#include <ccspi.h>
#include <SPI.h>
#include <CC3000_MDNS.h>
#include <aREST.h>
#include <avr/wdt.h>
#include <string.h>
#include "utility/debug.h"

// Macro Definitions
// These are the pins for the CC3000 chip if you are using a breakout board
#define ADAFRUIT_CC3000_IRQ   3   // IRQ: Interrupt Request Pin
// The below two pins could be any pins
#define ADAFRUIT_CC3000_VBAT  5   // VBAT: "Battery Voltage" to power the CC3000
#define ADAFRUIT_CC3000_CS    10  // CS: Chip Select

// Create CC3000 Instance
// Use hardware SPI for the remaining pins
// On an UNO, SCK = 13, MISO = 12, and MOSI = 11
Adafruit_CC3000 cc3000 = Adafruit_CC3000(ADAFRUIT_CC3000_CS, ADAFRUIT_CC3000_IRQ, ADAFRUIT_CC3000_VBAT,
                                         SPI_CLOCK_DIV2);

// Create aREST instance
aREST rest = aREST();

// Define WiFi SSID and password
#define WLAN_SSID       "KBHQ2"        // cannot be longer than 32 characters!
#define WLAN_PASS       "99KM8HPM4NRZC6QF"
// Security can be WLAN_SEC_UNSEC, WLAN_SEC_WEP, WLAN_SEC_WPA or WLAN_SEC_WPA2
#define WLAN_SECURITY   WLAN_SEC_WPA2

// The port to listen for incoming TCP connections 
#define LISTEN_PORT           80

// Server  that will run on our Arduino microcontroller
Adafruit_CC3000_Server restServer(LISTEN_PORT);

// DNS responder instance
MDNSResponder mdns;

/*******************************************/
/*******************************************/

// Begin by setting up the various Variables
int   analogVal;
int   temperature;
float resistance;

int   del         = 1000;         // Duration between temperature readings

const int B       =4275;          // B value of the thermistor
const int R0      = 10000;       // R0 = 10k
const int pinTempSensor = A0;     // Grove - Temperature Sensor connect to A5

void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);
    temperature = 24;     // Initial Test Default Temperature

    // Exposing the variables to the aREST API
    rest.variable("temperature", &temperature);

    // Define the Name & ID of our device
    rest.set_id("007");
    rest.set_name("Temp Station");

    // Initialize the CC3000 module
    if (!cc3000.begin())
    {
      while(1);
    }

    // Setup the CC3000 & get connected to the network.
    if (!cc3000.connectToAP(WLAN_SSID, WLAN_PASS, WLAN_SECURITY)) 
    {
      while(1);
    }
    // Checking DHCP Connections
    while (!cc3000.checkDHCP())
    {
      delay(100);
    }

    while (!displayConnectionDetails())
    {
      delay(del);
    }

      // Start multicast DNS responder
    if (!mdns.begin("arduino", cc3000)) 
    {
      while(1); 
    }
   
    // Start server
    restServer.begin();
    Serial.println(F("Listening for connections..."));

    // Enable watchdog to help reset the system in unusual or extreme circumstances.
    wdt_enable(WDTO_8S);
}

void loop() {
  // put your main code here, to run repeatedly:
  analogVal=analogRead(pinTempSensor);
  resistance=(float)(1023-analogVal)*R0/analogVal; 
  temperature=1/(log(resistance/R0)/B+1/298.15)-273.15;
  delay(del);
  Serial.println(temperature);

  // Handle any multicast DNS requests
  mdns.update();
  
  // Handle REST calls
  Adafruit_CC3000_ClientRef client = restServer.available();
  rest.handle(client);
  wdt_reset();

  // Check connection, reset if connection is lost
  if(!cc3000.checkConnected()){while(1){}}
  wdt_reset();


}


// Print connection details of the CC3000 chip
bool displayConnectionDetails(void)
{
  uint32_t ipAddress, netmask, gateway, dhcpserv, dnsserv;
  
  if(!cc3000.getIPAddress(&ipAddress, &netmask, &gateway, &dhcpserv, &dnsserv))
  {
    Serial.println(F("Unable to retrieve the IP Address!\r\n"));
    return false;
  }
  else
  {
    Serial.print(F("\nIP Addr: ")); 
    cc3000.printIPdotsRev(ipAddress);
/*    Serial.print(F("\nNetmask: ")); cc3000.printIPdotsRev(netmask);
    Serial.print(F("\nGateway: ")); cc3000.printIPdotsRev(gateway);
    Serial.print(F("\nDHCPsrv: ")); cc3000.printIPdotsRev(dhcpserv);
    Serial.print(F("\nDNSserv: ")); cc3000.printIPdotsRev(dnsserv);
    Serial.println();*/
    Serial.println();
    return true;
  }
}
