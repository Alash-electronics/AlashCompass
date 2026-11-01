// AlashCompass: «в какую сторону смотрит плата». Печатает сторону света при каждой смене, а не 100 раз в секунду.
// Удобно для робота, который должен ехать на север. setLanguage меняет язык названий.
#include <AlashCompass.h>

AlashCompass compass;
String last;

void setup() {
  Serial.begin(115200);
  if (!compass.begin()) {
    Serial.println(compass.errorText());
    while (true) delay(1000);
  }
  compass.setLanguage(AlashCompass::LANG_RU);   // LANG_KK: қазақша, LANG_EN: English
}

void loop() {
  String now = String(compass.direction());
  if (now != last) {
    last = now;
    Serial.print(now);
    Serial.print(F("  ("));
    Serial.print(compass.directionShort());
    Serial.println(')');
  }
  delay(100);
}
