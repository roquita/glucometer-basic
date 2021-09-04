
// headers for movil app
#include "BluetoothSerial.h"
BluetoothSerial bt;

void setup() {
  // setup for desktop app
  Serial.begin(9600);

  // setup for movil app
  bt.begin("glucometro"); 
}

void loop() {
  
  uint32_t val = random(50, 250);  

  // print on desktop app
  Serial.println(val);

  // print on movil app
  bt.println(val);
  
  delay(1000);
}
