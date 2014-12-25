/*
AT+CFUN=0    minimum functionality              0.796 mA (sleep mode)
AT+CFUN=1    full functionality (default)       1.02
AR+CFUN=4    flight mode (disable RF function)  0.892

user can control SIM800L module to enter or exit the sleep mode (AT+CSCLK=1) by DTR signal.
When DTR is in high level and without interrupt (on air and hardware such as GPIO interrupt or data in serial port),
SIM800L will enter sleep mode automatically. In this mode, SIM800L can still receive os SMS from network but the serial port is not accessible.


WAKE UP SIM800L from sleep mode (AT+CSCLK=1)
when SIM800L is in sleep mode (AT+CSCLSK=1), the folowwing methods can wake up the module :
* pull down DTR pin
  the serial port will be active after DTR pin is pulled to low level for about 50ms
* receive a voice or data call from network
* receive a SMS from network
* receive external interrupt
 
*/



// **** INCLUDES *****
#include "LowPower.h"
#include <Adafruit_NeoPixel.h>
#include <SoftwareSerial.h>
#include <Adafruit_FONA.h>



// Use pin 2 as wake up pin
#define wakeUpPin    3
#define LED          13

#define LED_RGB_PWR  4
#define LED_RGB      5

#define FONA_KEY     8
#define FONA_RX      10
#define FONA_TX      11
#define FONA_RST     6
#define FONA_PWR     9

// this is a large buffer for replies
//char replybuffer[255];

char pinCode[] = "2501";
char userName[] = "Client2000";
char destGilles[] = "33665724761";
char destJulien[] = "33699152929";
char destFabien[] = "33615133383";
char message[] = "ALERT from Louis-Zero !";  
char url[140];
char apnName[] = "free";
char apnUser[] = "21731190";
char apnPwd[] = "Oiz#106r";



SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);
Adafruit_FONA fona = Adafruit_FONA(&fonaSS, FONA_RST);



// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(1, LED_RGB, NEO_GRB + NEO_KHZ800);



uint8_t readline(char *buff, uint8_t maxbuff, uint16_t timeout = 0);

void wakeUp()
{
  // Just a handler for the pin interrupt.   
}

/*
void flushSerial() {
  while (Serial.available()) 
    Serial.read();
}
*/

void setup()
{
  // Configure wake up pin as input.
  // This will consumes few uA of current.
  pinMode(wakeUpPin, INPUT);  

  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  pinMode(LED_RGB_PWR, OUTPUT);
  digitalWrite(LED_RGB_PWR, LOW);

  pinMode(FONA_PWR, OUTPUT);
  digitalWrite(FONA_PWR, LOW);
  
  sprintf(url, "http://squirrel35.ddns.net/?user=%s&num=%s", userName, destGilles);
  
  // INIT ////////////////////////
  
  // ------ POWER ON ------
  digitalWrite(FONA_PWR, HIGH);
  
  strip.setPixelColor(0, 0, 0, 255);
  strip.show();
  
  // See if the FONA is responding
  //if (! fona.begin(4800)) {  // make it slow so its easy to read!
  if (! fona.begin(57600)) {  // make it slow so its easy to read!
    //Serial.println(F("Couldn't find FONA"));
    while (1);
  }
  //Serial.println(F("FONA is OK"));

  // Optionally configure a GPRS APN, username, and password.
  // You might need to do this to access your network's GPRS/data
  // network.  Contact your provider for the exact APN, username,
  // and password values.  Username and password are optional and
  // can be removed, but APN is required.
  //fona.setGPRSNetworkSettings(F("your APN"), F("your username"), F("your password"));
  fona.setGPRSNetworkSettings((const __FlashStringHelper*)apnName, (const __FlashStringHelper*)apnUser, (const __FlashStringHelper*)apnPwd);
  fona.setGPRSNetworkSettings(F("free"), F("21731190"), F("Oiz#106r"));

  // Optionally configure HTTP gets to follow redirects over SSL.
  // Default is not to follow SSL redirects, however if you uncomment
  // the following line then redirects over SSL will be followed.
  //fona.setHTTPSRedirect(true);
  
  // ------ UNLOCK SIM ------
  strip.setPixelColor(0, 255, 0, 0);
  strip.show();

  // Unlock the SIM with a PIN code
  //Serial.println(pinCode);
  //Serial.print(F("Unlocking SIM card: "));
  if (! fona.unlockSIM(pinCode)) {
    //Serial.println(F("Failed"));
  } 
  else {
    //Serial.println(F("OK!"));
  }
  
  // ------ NETWORK ------
  strip.setPixelColor(0, 0, 255, 0);
  strip.show();
  /*
  delay(100);

  uint8_t n = 0;
  while(n < 5) 
  {
    n = fona.getNetworkStatus();  // Read the Network / Cellular Status
  }
  //Serial.println("done");
  
  
  // ------ ACTIVATE GPRS ------
  strip.setPixelColor(0, 255, 255, 255);
  strip.show();
  
  delay(100);
  
  while (!fona.enableGPRS(true))  {
    //Serial.println(F("GPRS Failed to turn on"));
    delay(200);
  }
  */
  
  delay(3000);
  
  // power OFF the LED_RGB
  digitalWrite(LED_RGB_PWR, LOW);
  
  // Enter sleep mode
  fona.enterSleepMode();  // AT+CSCLK=1
}

void loop() 
{
  // Allow wake up pin to trigger interrupt on low.
  attachInterrupt(1, wakeUp, RISING);

  // Enter power down state with ADC and BOD module disabled.
  // Wake up when wake up pin is low.
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF); 

  // Disable external pin interrupt on wake up pin.
  detachInterrupt(1); 

  // Do something here
  // Example: Read sensor, data logging, data transmission.
  // ------------------------------------------------------------

  // power ON the LED_RGB
  digitalWrite(LED_RGB_PWR, HIGH);

  strip.begin();
  strip.setBrightness(16);
  strip.show(); // Initialize all pixels to 'off'

  //-----------------------------

  //Serial.begin(115200);
  //Serial.println(F("FONA basic test"));
  //Serial.println(F("Initializing....(May take 3 seconds)"));


  // INIT ////////////////////////////
 /* 
  // ------ POWER ON ------
  digitalWrite(FONA_PWR, HIGH);
  
  strip.setPixelColor(0, 0, 0, 255);
  strip.show();
  
  // See if the FONA is responding
  if (! fona.begin(4800)) {  // make it slow so its easy to read!
    //Serial.println(F("Couldn't find FONA"));
    while (1);
  }
  //Serial.println(F("FONA is OK"));

  // Optionally configure a GPRS APN, username, and password.
  // You might need to do this to access your network's GPRS/data
  // network.  Contact your provider for the exact APN, username,
  // and password values.  Username and password are optional and
  // can be removed, but APN is required.
  //fona.setGPRSNetworkSettings(F("your APN"), F("your username"), F("your password"));
  fona.setGPRSNetworkSettings(F("free"), F("21731190"), F("Oiz#106r"));
  
  // Optionally configure HTTP gets to follow redirects over SSL.
  // Default is not to follow SSL redirects, however if you uncomment
  // the following line then redirects over SSL will be followed.
  //fona.setHTTPSRedirect(true);
  
  // ------ UNLOCK SIM ------
  strip.setPixelColor(0, 255, 0, 0);
  strip.show();

  // Unlock the SIM with a PIN code
  //Serial.println(pinCode);
  //Serial.print(F("Unlocking SIM card: "));
  if (! fona.unlockSIM(pinCode)) {
    //Serial.println(F("Failed"));
  } 
  else {
    //Serial.println(F("OK!"));
  }
  
  // ------ NETWORK ------
  strip.setPixelColor(0, 0, 255, 0);
  strip.show();
  
  delay(100);

  uint8_t n = 0;
  while(n < 5) {
    n = fona.getNetworkStatus();  // Read the Network / Cellular Status
    
    //Serial.print(F("Network status ")); 
    //Serial.print(n);
    //Serial.print(F(": "));
    //if (n == 0) Serial.println(F("Not registered"));
    //if (n == 1) Serial.println(F("Registered (home)"));
    //if (n == 2) Serial.println(F("Not registered (searching)"));
    //if (n == 3) Serial.println(F("Denied"));
    //if (n == 4) Serial.println(F("Unknown"));
    //if (n == 5) Serial.println(F("Registered roaming"));
  }
  //Serial.println("done");
  */


  // WAPKE UP /////////////////////////
  digitalWrite(FONA_RX, HIGH);
  delay(100);
  digitalWrite(FONA_RX, LOW);
  delay(100);
  
  delay(500);
  
  // PROCESS //////////////////////////
  
  // ------ SEND SMS #1 ------
  strip.setPixelColor(0, 0, 0, 255);
  strip.show();
  
  // send an SMS!
  //Serial.print(F("Send to #"));
  //Serial.println(sendto);
  //Serial.println(message);
//  if (!fona.sendSMS(destGilles, message)) {
  if (!fona.sendSMS(destJulien, message)) {
    //Serial.println(F("Failed"));
  } 
  else {
    //Serial.println(F("Sent!"));
  }
  
  // ------ SEND SMS #2 ------
  strip.setPixelColor(0, 255, 0, 0);
  strip.show();

  if (!fona.sendSMS(destFabien, message)) {
    //Serial.println(F("Failed"));
  } 
  else {
    //Serial.println(F("Sent!"));
  }
  
  
  // ------ ACTIVATE GPRS ------
  /*
  strip.setPixelColor(0, 255, 255, 255);
  strip.show();
  
  delay(100);
  
  while (!fona.enableGPRS(true))  {
    //Serial.println(F("GPRS Failed to turn on"));
    delay(200);
  }
  */
  
  // ------ SEND URL ------
  strip.setPixelColor(0, 0, 255, 255);
  strip.show();
   
  // read website URL
  uint16_t statuscode;
  int16_t length;
  Serial.println(url);
  if (!fona.HTTP_GET_start(url, &statuscode, (uint16_t *)&length)) {
    //Serial.println(F("Failed"));
  } 
  else {
    //Serial.println(F("Sent!"));
  }
  fona.HTTP_GET_end();
  
  // ------ SHUTDOWN ------
  strip.setPixelColor(0, 255, 0, 0);
  strip.show();
  /*
  while (!fona.enableGPRS(false))  {
    //Serial.println(F("GPRS Failed to turn off"));
    delay(200);
  }
  */
  
  //Serial.println(F("bybye"));
  
  digitalWrite(LED_RGB_PWR, LOW);
  /*
  // power OFF the FONA
  digitalWrite(FONA_PWR, LOW);
  delay(1000);
  */
  
  // Enter sleep mode
  fona.enterSleepMode();  // AT+CSCLK=1
}


