#include "generated/TcmTouch-XPT2046_menu.h"
/*******************************************************************
    A touch screen test for the ESP32 Cheap Yellow Display.

    https://github.com/witnessmenow/ESP32-Cheap-Yellow-Display

    If you find what I do useful and would like to support me,
    please consider becoming a sponsor on Github
    https://github.com/sponsors/witnessmenow/

    Written by Brian Lough
    YouTube: https://www.youtube.com/brianlough
    Twitter: https://twitter.com/witnessmenow
 *******************************************************************/

// Make sure to copy the UserSetup.h file into the library as
// per the Github Instructions. The pins are defined in there.

// ----------------------------
// Standard Libraries
// ----------------------------

#include <SPI.h>

// ----------------------------
// Additional Libraries - each one of these will need to be installed.
// ----------------------------

#include <TFT_eSPI.h>
// A library for interfacing with LCD displays
//
// Can be installed from the library manager (Search for "TFT_eSPI")
//https://github.com/Bodmer/TFT_eSPI

#include "WORKAROUND_TCMENU_WITH_ESP32_CYD.h"
#ifdef WORKAROUND_TCMENU_WITH_ESP32_CYD
#include <XPT2046_Touchscreen.h>
// A library for interfacing with the touch screen
//
// Can be installed from the library manager (Search for "XPT2046")
//https://github.com/PaulStoffregen/XPT2046_Touchscreen
// ----------------------------
// Touch Screen pins
// ----------------------------
// The CYD touch uses some non default
// SPI pins
#define XPT2046_IRQ 36
#define XPT2046_MOSI 32
#define XPT2046_MISO 39
#define XPT2046_CLK 25
#define XPT2046_CS 33
// ----------------------------
SPIClass mySpi = SPIClass(VSPI);
//XPT2046_Touchscreen touchDevice(XPT2046_CS, XPT2046_IRQ); // Dont need this as tcMenuDesigner puts it in *_menu.cpp
#endif // WORKAROUND_TCMENU_WITH_ESP32_CYD

TFT_eSPI tft = TFT_eSPI();

#include "..\..\Credentials\Credentials.h"

#define DEF_WIFI_STA
#ifdef DEF_WIFI_STA
#define NETWORK_NETCOMM
#ifdef NETWORK_NETCOMM
char ssid[] = netcomm24_wifi_ssid;
char pw[] = netcomm24_wifi_pw;
#endif
IPAddress netmask(255, 255, 255, 0); 	// default
#define DEF_WIFI_GWAY  192,168,20,1 	// default gateway
#define DEF_WIFI_IP2   192,168,20,13  // lucky for some?
#endif

#define SCREEN_ROTATION 1

//#define MANUAL_CALIBRATION // enable to access resistive touch calibration data via a separate build and run.


void setup() {
  Serial.begin(115200);

#ifdef WORKAROUND_TCMENU_WITH_ESP32_CYD
  // Start the SPI for the touch screen and init the TS library
  mySpi.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);  // provide ALL SPI3 (VSPI) pin definitions.
  touchDevice.begin(mySpi);
  touchDevice.setRotation(SCREEN_ROTATION);

  // step 1. run with calibration off and get the actual min and max values if corrections need to be made
  // step 2. put the corrections into the value below, xmin, xmax, ymin, ymax and try the program again.
  //touchScreen.calibrateMinMaxValues(0.15F, 0.75F, 0.06F, 0.91F); // some example values.
  touchScreen.calibrateMinMaxValues(0.099F, 0.904F, 0.081F, 0.906F); // from ESP_CYD cal build via MANUAL_CALIBRATION
  touchScreen.start();
#endif // WORKAROUND_TCMENU_WITH_ESP32_CYD

  // Start the tft display and set it to black
  tft.init();
  tft.setRotation(SCREEN_ROTATION); //This is the display in landscape

#ifdef MANUAL_CALIBRATION // enable to access resistive touch calibration data
  // Clear the screen before writing to it
  tft.fillScreen(TFT_BLACK);
  int x = 320 / 2; // center of display
  int y = 100;
  int fontSize = 2;
  tft.drawCentreString("Touch Screen to Start", x, y, fontSize);
#endif

#define ALPHALIMA_DELAY_MS 2000

#ifdef DEF_WIFI_STA
	IPAddress ip_gway (DEF_WIFI_GWAY);
	IPAddress ip_static (DEF_WIFI_IP2);
	WiFi.mode(WIFI_STA);
	WiFi.config(ip_static, ip_gway, netmask);
	WiFi.begin(ssid, pw);
	delay(ALPHALIMA_DELAY_MS); // THIS DELAY IS VERY IMPORTANT : comment from AlphaLima www.LK8000.com ?
#if(0)
	Serial.print("ESP Board MAC Address:  ");
  Serial.println(WiFi.macAddress());
#endif
#endif

  setupMenu();
} // setup


void printTouchToSerial(TS_Point p) {
  Serial.print("Pressure = ");
  Serial.print(p.z);
  Serial.print(", x = ");
  Serial.print(p.x);
  Serial.print(", y = ");
  Serial.print(p.y);
  Serial.println();
} // printTouchToSerial


void printTouchToDisplay(TS_Point p) {
  // Clear screen first
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  int x = 320 / 2; // center of display
  int y = 100;
  int fontSize = 2;
  String temp = "Pressure = " + String(p.z);
  tft.drawCentreString(temp, x, y, fontSize);
  y += 16;
  temp = "X = " + String(p.x);
  tft.drawCentreString(temp, x, y, fontSize);
  y += 16;
  temp = "Y = " + String(p.y);
  tft.drawCentreString(temp, x, y, fontSize);
} // printTouchToDisplay


void loop() {
  taskManager.runLoop();

#ifdef MANUAL_CALIBRATION // enable to access resistive touch calibration data
  if (touchDevice.tirqTouched() && touchDevice.touched()) {
    TS_Point p = touchDevice.getPoint();
    printTouchToSerial(p);
    printTouchToDisplay(p);
    delay(100);
    //delay(1000);
  }
#endif

} // loop



void CALLBACK_FUNCTION onChangeTcmDebugLED(int id) {
    // TODO - your menu change code
}


void CALLBACK_FUNCTION onChangeTcmBaseTCW(int id) {
    // TODO - your menu change code
}


void CALLBACK_FUNCTION onChangeTcmBaseCCW(int id) {
    // TODO - your menu change code
}


void (* resetFunc) (void) = 0; // Arduino Forum alto777

void CALLBACK_FUNCTION onChangeTcmRestart(int id) {

//#ifndef DEF_ETHERMEGA2560
//	ESP.restart();
//#else
	resetFunc();
//#endif

}


// END_OF_FILE
