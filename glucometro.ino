
// headers for movil app
#include "BluetoothSerial.h"
BluetoothSerial bt;

// headers for screen
#include <Arduino.h>
#include <U8x8lib.h>
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif

// global vars for movil app

// global vars for screen
U8X8_SSD1306_128X64_NONAME_4W_SW_SPI u8x8( /*clock=*/ 15,  /*data= */2, /* cs=*/ -1, /* dc=*/ 5, /* reset=*/ 4);

void setup() {
  // setup for desktop app
  Serial.begin(9600);

  // setup for movil app
  bt.begin("glucometro");

  // setup for screen
  u8x8.begin();
  u8x8.setPowerSave(0);
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  u8x8.drawString(3, 0, "Glucometro");
  u8x8.setFont(u8x8_font_8x13B_1x2_r);
  u8x8.drawString(10, 3, "mg/dL");
}

void loop() {

  int32_t val = random(-50, 300);
  val = val < 0 ? 0 : (val > 999 ? 999 : val) ;

  // print on desktop app
  Serial.println(val);

  // print on movil app
  bt.println(val);

  // print on screen
  char num[30] = {0} ;
  sprintf(num, "%3i", val);

  char result[30] = {0};
  if (val >= 0 && val <= 53)
    sprintf(result, "%s", "hipoglicemia  N2");
  else if ( val >= 54 && val <= 69)
    sprintf(result, "%s", "hipoglicemia  N1");
  else if ( val >= 70 && val <= 180)
    sprintf(result, "%s", "homoglicemia  OK");
  else if ( val >= 181 && val <= 250)
    sprintf(result, "%s", "hiperglicemia N1");
  else if ( val >= 251 )
    sprintf(result, "%s", "hiperglicemia N2");

  u8x8.setFont(u8x8_font_8x13B_1x2_r);
  u8x8.drawString(0, 6, result);
  u8x8.setFont(u8x8_font_px437wyse700b_2x2_r);
  u8x8.drawString(2, 3, num);

  delay(2000);
}
