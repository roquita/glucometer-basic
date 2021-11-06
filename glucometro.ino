
// headers for movil app
#include "BluetoothSerial.h"
BluetoothSerial bt;

// headers for screen
#include <Arduino.h>
#include <U8x8lib.h>
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif

// headers fro ad5933
#include <Wire.h>
#include "AD5933.h"
#define START_FREQ  (50000)
#define FREQ_INCR   (1000)
#define NUM_INCR    (1)
#define REF_RESIST  (17530)

// global vars for movil app

// global vars for screen
U8X8_SSD1306_128X64_NONAME_4W_SW_SPI u8x8( /*clock=*/ 15,  /*data= */2, /* cs=*/ -1, /* dc=*/ 5, /* reset=*/ 4);

// global vars for ad5933
double gain[NUM_INCR + 1] = {0.000000010256, 0.000000010264};
int phase[NUM_INCR + 1] = {1, 1};

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

  // setup for ad5933
  Wire.begin();
  if (!(AD5933::reset() &&
        AD5933::setInternalClock(true) &&
        AD5933::setStartFrequency(START_FREQ) &&
        AD5933::setIncrementFrequency(FREQ_INCR) &&
        AD5933::setNumberIncrements(NUM_INCR) &&
        AD5933::setPGAGain(PGA_GAIN_X1)))
  {
    Serial.println("FAILED in initialization!");
    while (true) ;
  }
}

void loop() {
  // read impedance from ad5933
  double impedance = frequencySweepEasy() * 1.015 - 246.133;
  //Serial.printf("impedancia: %.3f ||", impedance);

  // convert impedance to glucose level
  int32_t val = impedance_to_glucose(impedance);

  // bound glucose level
  val = val < 0 ? 0 : (val > 999 ? 999 : val) ;
  //Serial.printf("glucosa: %i\n", val);

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

  delay(100);
}
double frequencySweepEasy() {
  // Create arrays to hold the data
  int real[NUM_INCR + 1], imag[NUM_INCR + 1];

  // Perform the frequency sweep
  if (AD5933::frequencySweep(real, imag, NUM_INCR + 1)) {
    // Print the frequency data
    int cfreq = START_FREQ / 1000;
    double impedance = 0.0;
    for (int i = 0; i < NUM_INCR + 1; i++, cfreq += FREQ_INCR / 1000) {
      // Print raw frequency data
      /*
        Serial.print(cfreq);
        Serial.print(": R=");
        Serial.print(real[i]);
        Serial.print("/I=");

        Serial.print(imag[i]);
      */
      // Compute impedance
      double magnitude = sqrt(pow(real[i], 2) + pow(imag[i], 2));
      impedance = 1 / (magnitude * gain[i]);
      /*
        Serial.print("  |Z|=");
        Serial.println(impedance);*/

    }
    //Serial.println("Frequency sweep complete!");
    return impedance;
  } else {
    //Serial.println("Frequency sweep failed...");
    return -1.0;
  }
}
int32_t impedance_to_glucose(double impedance) {
  const double m = 21.923076923076923076923076923077;
  const double b = 0.0;

  return (int32_t)((impedance / 1000.0) * m + b);
}
