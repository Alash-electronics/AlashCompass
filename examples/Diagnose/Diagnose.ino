// [ru] AlashCompass: компас врёт или не найден? Скетч покажет устройства на шине I2C, тип микросхемы,
//      силу поля (должна быть 25-65 мкТл) и подскажет, что проверить.
// [kk] AlashCompass: компас өтірік айтады немесе табылмай ма? Скетч I2C шинасындағы құрылғыларды, микросхема түрін,
//      өріс күшін (25-65 мкТл болуы керек) көрсетеді және нені тексеру керегін айтады.
// [en] AlashCompass: the compass lies or is not found? The sketch shows the devices on the I2C bus, the chip type,
//      the field strength (should be 25-65 uT) and tells what to check.
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
