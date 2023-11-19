#include <ETH.h>

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

#include <ETH.h>
#include <HTTPClient.h>
#include <time.h>
//RFID-----------------------------
#include <SPI.h>
#include <MFRC522.h>
//OLED-----------------------------
#include "Includes/OLED/LCD_Driver.h"
#include <GUI_Paint.h>
#include "/Includes/OLED/image.h"

//#include <Adafruit_GFX.h>
//#include <Adafruit_SSD1306.h>
//************************************************************************
#define SS_PIN  4
#define RST_PIN 5
// Declaration for ST7789V2 1.69inch LCD Module display
// Old Display INFO connected using software I2C pins are(22 SCL, 21 SDA)

#define SCREEN_WIDTH 240 // OLED display width, in pixels
#define SCREEN_HEIGHT 280 // OLED display height, in pixels
#define OLED_RESET     0 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
//************************************************************************
MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance.
//************************************************************************
// Ethernet Setup
#define ETH_TYPE        ETH_PHY_TLK110
#define ETH_ADDR        31
#define ETH_MDC_PIN     23
#define ETH_MDIO_PIN    18
#define ETH_POWER_PIN   17
#define ETH_CLK_MODE    ETH_CLOCK_GPIO0_IN


static bool eth_connected = false;

void onEvent(arduino_event_id_t event, arduino_event_info_t info)
{
  switch (event) {
    case ARDUINO_EVENT_ETH_START:
      Serial.println("ETH Started");
      //set eth hostname here
      ETH.setHostname("ClockFulfillment");
      break;
    case ARDUINO_EVENT_ETH_CONNECTED:
      Serial.println("ETH Connected");
      break;
    case ARDUINO_EVENT_ETH_GOT_IP:
      Serial.println("ETH Got IP");
      ETH.printInfo(Serial);
      eth_connected = true;
      break;
    case ARDUINO_EVENT_ETH_LOST_IP:
      Serial.println("ETH Lost IP");
      eth_connected = false;
      break;
    case ARDUINO_EVENT_ETH_DISCONNECTED:
      Serial.println("ETH Disconnected");
      eth_connected = false;
      break;
    case ARDUINO_EVENT_ETH_STOP:
      Serial.println("ETH Stopped");
      eth_connected = false;
      break;
    default:
      break;
  }
}

void testClient(const char * host, uint16_t port)
{
  Serial.print("\nconnecting to ");
  Serial.println(host);

  WiFiClient client;
  if (!client.connect(host, port)) {
    Serial.println("connection failed");
    return;
  }
  client.printf("GET / HTTP/1.1\r\nHost: %s\r\n\r\n", host);
  while (client.connected() && !client.available());
  while (client.available()) {
    Serial.write(client.read());
  }

  Serial.println("closing connection\n");
  client.stop();
}
//************************************************************************
// Ethernet Setup Stop
//************************************************************************
int timezone = -6 * time_dst;   // Timezone for Wichita, Kansas
int time_dst = 3600;
String getData, Link;
String OldCardID = "";
unsigned long previousMillis1 = 0;
unsigned long previousMillis2 = 0;
String URL = "http://192.168.29.35/getdata.php"; // Server domain or IP
//*************************Biometric Icons*********************************
void setup() {
  delay(1000);
  Serial.begin(115200);
  SPI.begin();  // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522 card
  //-----------initiate OLED display-------------
{
    Config_Init();
    LCD_Init();

    LCD_SetBacklight(100);
}
  //-----------initiate OLED display End-------------

  display.display();
  delay(2000); // Pause for 2 seconds
  display.clearDisplay();
  //---------------------------------------------
  // Initialize Ethernet or other network connection here if needed
  //---------------------------------------------
{
  Serial.begin(115200);
  WiFi.onEvent(onEvent);
  ETH.begin(ETH_TYPE, ETH_ADDR, ETH_MDC_PIN, ETH_MDIO_PIN, ETH_POWER_PIN, ETH_CLK_MODE);
}

  configTime(timezone, time_dst, "pool.ntp.org","time.nist.gov");
}
//************************************************************************
void loop() {
  // Main loop code
  // Ensure network connectivity if necessary
  //---------------------------------------------
  if (millis() - previousMillis1 >= 1000) {
    previousMillis1 = millis();
    display.clearDisplay();

    time_t now = time(nullptr);
    struct tm* p_tm = localtime(&now);
    displayTime(p_tm);
  }
  //---------------------------------------------
  if (millis() - previousMillis2 >= 15000) {
    previousMillis2 = millis();
    OldCardID="";
  }
  delay(50);
  //---------------------------------------------
  checkForNewCard();

}
//************************************************************************

// Function definitions for displayTime, checkForNewCard, SendCardID, etc.

//************************************************************************
// Add any necessary functions here, such as for handling Ethernet connectivity
//=======================================================================
