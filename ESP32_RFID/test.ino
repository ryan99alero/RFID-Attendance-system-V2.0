  /* -----------------------------------------------------------------------------
  - Project: RFID attendance system using ESP32
  - Author:  https://www.youtube.com/ElectronicsTechHaIs
  - Date:  6/03/2020
   -----------------------------------------------------------------------------
  This code was created by Electronics Tech channel for
  the RFID attendance project with ESP32.
   ---------------------------------------------------------------------------*/
//*******************************libraries********************************
//ESP32----------------------------
#include <WiFi.h>
#include <HTTPClient.h>
#include <time.h>
//RFID-----------------------------
#include <SPI.h>
#include <MFRC522.h>
//OLED-----------------------------
#include <Wire.h>
#include <Adafruit_GFX.h>          //https://github.com/adafruit/Adafruit-GFX-Library
#include <Adafruit_SSD1306.h>      //https://github.com/adafruit/Adafruit_SSD1306
//************************************************************************
//Start Ethernet Setup
//************************************************************************
#define ETH_TYPE        ETH_PHY_TLK110
#define ETH_ADDR        31
#define ETH_MDC_PIN     23
#define ETH_MDIO_PIN    18
#define ETH_POWER_PIN   17
#define ETH_CLK_MODE    ETH_CLOCK_GPIO0_IN
static bool eth_connected = false;
//************************************************************************
//End Ethernet Setup
//************************************************************************

#define SS_PIN  4
#define RST_PIN 5
// Declaration for SSD1306 display connected using software I2C pins are(22 SCL, 21 SDA)
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     0 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
//************************************************************************
MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance.
//************************************************************************
int timezone = -6 * 3600;   //Replace "x" your timezone.
int time_dst = 3600;
String getData, Link;
String OldCardID = "";
unsigned long previousMillis1 = 0;
unsigned long previousMillis2 = 0;
String URL = "http://YourComputerIP/rfidattendance/getdata.php"; //computer IP or the server domain
//*************************Biometric Icons*********************************
void setup() {
  delay(1000);
  Serial.begin(115200);
  SPI.begin();  // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522 card
  //-----------initiate OLED display-------------
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  configTime(timezone, time_dst, "pool.ntp.org","time.nist.gov");
}
//************************************************************************
void loop() {
  //check if there's a connection to Wi-Fi or not
  if(!WiFi.isConnected()){
    connectToWiFi();    //Retry to connect to Wi-Fi
  }
  //---------------------------------------------
  if (millis() - previousMillis1 >= 1000) {
    previousMillis1 = millis();
    display.clearDisplay();

    time_t now = time(nullptr);
    struct tm* p_tm = localtime(&now);
    display.setTextSize(1);             // Normal 2:2 pixel scale
    display.setTextColor(WHITE);        // Draw white text
    display.setCursor(10,0);
    Serial.println(p_tm);
    display.setTextSize(4);             // Normal 2:2 pixel scale
    display.setTextColor(WHITE);        // Draw white text
    display.setCursor(0,21);
    if ((p_tm->tm_hour)<10) {display.print("0");display.print(p_tm->tm_hour);}
    else display.print(p_tm->tm_hour);
    display.print(":");
    if ((p_tm->tm_min)<10) {display.print("0");display.println(p_tm->tm_min);}
    else display.println(p_tm->tm_min);
    display.display();
  }
  //---------------------------------------------
  if (millis() - previousMillis2 >= 15000) {
    previousMillis2 = millis();
    OldCardID="";
  }
  delay(50);
  //---------------------------------------------
  //look for new card
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;//got to start of loop if there is no card present
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;//if read card serial(0) returns 1, the uid struct contains the ID of the read card.
  }
  String CardID ="";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    CardID += mfrc522.uid.uidByte[i];
  }
  //---------------------------------------------
  if( CardID == OldCardID ){
    return;
  }
  else{
    OldCardID = CardID;
  }
  //---------------------------------------------
//  Serial.println(CardID);
  SendCardID(CardID);
  delay(1000);
  display.clearDisplay();
}
//************send the Card UID to the website*************
void SendCardID( String Card_uid ){
  Serial.println("Sending the Card ID");
  if(WiFi.isConnected()){
    HTTPClient http;    //Declare object of class HTTPClient
    //GET Data
    getData = "?card_uid=" + String(Card_uid) + "&device_token=" + String(device_token); // Add the Card ID to the GET array in order to send it
    //GET methode
    Link = URL + getData;
    http.begin(Link); //initiate HTTP request   //Specify content-type header

    int httpCode = http.GET();   //Send the request
    String payload = http.getString();    //Get the response payload

//    Serial.println(Link);   //Print HTTP return code
    Serial.println(httpCode);   //Print HTTP return code
    Serial.println(Card_uid);     //Print Card ID
    Serial.println(payload);    //Print request response payload

    if (httpCode == 200) {
      if (payload.substring(0, 5) == "login") {
        String user_name = payload.substring(5);
    //  Serial.println(user_name);

        display.clearDisplay();
        display.setTextSize(2);             // Normal 2:2 pixel scale
        display.setTextColor(WHITE);        // Draw white text
        display.setCursor(15,0);             // Start at top-left corner
        display.print(F("Welcome"));
        display.setCursor(0,20);
        display.print(user_name);
        display.display();
      }
      else if (payload.substring(0, 6) == "logout") {
        String user_name = payload.substring(6);
    //  Serial.println(user_name);

        display.clearDisplay();
        display.setTextSize(2);             // Normal 2:2 pixel scale
        display.setTextColor(WHITE);        // Draw white text
        display.setCursor(10,0);             // Start at top-left corner
        display.print(F("Good Bye"));
        display.setCursor(0,20);
        display.print(user_name);
        display.display();
      }
      else if (payload == "succesful") {
        display.clearDisplay();
        display.setTextSize(2);             // Normal 2:2 pixel scale
        display.setTextColor(WHITE);        // Draw white text
        display.setCursor(5,0);             // Start at top-left corner
        display.print(F("New Card"));
        display.display();
      }
      else if (payload == "available") {
        display.clearDisplay();
        display.setTextSize(2);             // Normal 2:2 pixel scale
        display.setTextColor(WHITE);        // Draw white text
        display.setCursor(5,0);             // Start at top-left corner
        display.print(F("Free Card"));
        display.display();
      }
      delay(100);
      http.end();  //Close connection
    }
  }
}