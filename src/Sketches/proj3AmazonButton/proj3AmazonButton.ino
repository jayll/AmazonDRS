#include <WiFi101.h>
#include "AmazonDRS.h"

AmazonDRS DRS = AmazonDRS();

//WiFi creds ----------------------------------------------------------------------------------
char ssid[] = "Jay’s iPhone"; //  your network SSID
char pass[] = "cse321proj3";    // your network password
//------------------------------------------------------------------------------------------------------

#define slotNumber1 1 //This will vary for multi slot devices - dash buttons typically only serve one product/slot
#define slotNumber2 2 //This will vary for multi slot devices - dash buttons typically only serve one product/slot

const int blue_LED = 9;     //blue LED pin9
const int green_LED = 10;   //green LED pin10
const int yellow_LED = 11;  //yellow LED pin11
const int dashButton1 = 12; //push button pin12
const int dashButton2 = 6;  //push button pin6

static long buttonHigh = 12;    //millis of last button push for switch debouncing
static String slotStatus1 = "true"; //boolean which depicts if slot is available for replenishment
static String slotStatus2 = "true"; //boolean which depicts if slot is available for replenishment
static String slotId1 = "3feda534-dc9d-4faa-8ed5-50f8aa4ab7a5";    //unique slot id1
static String slotId2 = "a4573ea5-ba4b-42f5-8c98-68401cec8f0f";    //unique slot id2

int status = WL_IDLE_STATUS;

void setup() {
  WiFi.setPins(8, 7, 4, 2);
  Serial.begin(115200);

  pinMode(dashButton1, INPUT_PULLUP); //Set button1 as input
  pinMode(dashButton2, INPUT_PULLUP); //Set button2 as input

  pinMode(blue_LED, OUTPUT);  //Set blue_LED pin as output
  pinMode(green_LED, OUTPUT); //Set green_LED pin as output
  pinMode(yellow_LED, OUTPUT); //Set yellow_LED pin as output

  digitalWrite(blue_LED, LOW);  //Set blue_LED pin as low to ensure not lit
  digitalWrite(green_LED, LOW); //Set green_LED pin as low to ensure not lit
  digitalWrite(yellow_LED, LOW); //Set yellow_LED pin as low to ensure not lit

  WiFiSSLClient client; //the SSL client to reach amazon servers

  DRS.begin(&client); //Start up DRS

  //connect to WiFi
  Serial.println("Initializing DRS... connecting to WiFi");
  while (status != WL_CONNECTED) {
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);
    Serial.println(".");
    delay(3000);
    status = WiFi.status();
  }
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());
  digitalWrite(yellow_LED, HIGH); //turn on yellow_LED when connected to WIFI

  //initialize slots
  DRS.retrieveSubscriptionInfo();
  slotStatus1 = DRS.getSlotStatus(slotNumber1);
  slotStatus2 = DRS.getSlotStatus(slotNumber2);

  while (1) {
    if(buttonPushed() == 1){
      if (slotStatus1 == "true") { //if the product in slot are available
        //we have a match! replenish the products associated with that slot!
        digitalWrite(green_LED, HIGH);
        slotId1 = DRS.getSlotId(slotNumber1);
        Serial.println(slotId1);
        DRS.requestReplenishmentForSlot(slotId1);
        delay(2000);
        digitalWrite(green_LED, LOW);
        Serial.println("COMPLETED");
      } else {
        Serial.print("Sorry, slot ");
        Serial.print(slotId1);
        Serial.println(" is not available at this time");
      }
    }else if (buttonPushed() == 2){
      if (slotStatus2 == "true") { //if the product in slot are available
        //we have a match! replenish the products associated with that slot!
          digitalWrite(blue_LED, HIGH);
         slotId2 = DRS.getSlotId(slotNumber2);
        Serial.println(slotId2);
        DRS.requestReplenishmentForSlot(slotId2);
        delay(2000);
        digitalWrite(blue_LED, LOW);
        Serial.println("COMPLETED");
      } else {
        Serial.print("Sorry, slot ");
        Serial.print(slotId2);
        Serial.println(" is not available at this time");
      }
    }
  }

} 

void loop() {

}

int buttonPushed(void){
  int buttonState1 = digitalRead(dashButton1);
  int buttonState2 = digitalRead(dashButton2);

  if (!buttonState1 && ((millis() - buttonHigh) > 5000)){
    buttonHigh = millis();
    Serial.println("Button1 pressed!!");
    return 1;
  } else if (!buttonState2 && ((millis() - buttonHigh) > 5000)){
    buttonHigh = millis();
    Serial.println("Button2 pressed!!");
    return 2;
  } else {
    return 0;
  }
}
