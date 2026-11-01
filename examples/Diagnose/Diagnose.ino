// AlashCompass: компас врёт или не найден? Скетч покажет устройства на шине I2C, тип микросхемы,
// силу поля (должна быть 25-65 мкТл) и подскажет, что проверить.
#include <AlashCompass.h>

AlashCompass compass;

void setup() {
  Serial.begin(115200);
  delay(500);
  compass.setLanguage(AlashCompass::LANG_RU);   // LANG_KK: қазақша, LANG_EN: English
  compass.begin();
  compass.diagnose(Serial);
}

void loop() {}
