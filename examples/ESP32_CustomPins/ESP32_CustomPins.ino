// [ru] AlashCompass на ESP32 со своими пинами I2C: begin(SDA, SCL).
// [kk] AlashCompass ESP32-де өз I2C пиндерімен: begin(SDA, SCL).
// [en] AlashCompass on ESP32 with its own I2C pins: begin(SDA, SCL).
#include <AlashCompass.h>

AlashCompass compass;

void setup() {
  Serial.begin(115200);
  if (!compass.begin(21, 22)) {           // SDA = GPIO21, SCL = GPIO22
    Serial.println(compass.errorText());
    while (true) delay(1000);
  }
}

void loop() {
  Serial.println(compass.heading(), 0);
  delay(200);
}
