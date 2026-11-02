// [ru] AlashCompass: «в какую сторону смотрит плата». Печатает сторону света при каждой смене, а не 100 раз в секунду.
//      Удобно для робота, который должен ехать на север. setLanguage меняет язык названий.
// [kk] AlashCompass: «тақта қай жаққа қарайды». Әлем тарапын секундына 100 рет емес, әр өзгерген сайын басып шығарады.
//      Солтүстікке жүруі керек робот үшін ыңғайлы. setLanguage атаулар тілін өзгертеді.
// [en] AlashCompass: "which way the board faces". Prints the compass direction on every change, not 100 times a second.
//      Handy for a robot that must drive north. setLanguage changes the language of the names.
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
