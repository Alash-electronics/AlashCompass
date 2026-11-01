// AlashCompass: калибровка. Рядом с датчиком всегда есть железо и провода, и без калибровки курс врёт на 10-40 градусов.
// Когда сообщение появится, плавно вращайте плату по кругу в горизонтальной плоскости и немного наклоняйте её (15 секунд).
// В конце библиотека напечатает строку setCalibration(...). Вставьте её в setup() после begin(): калибровка сохранится в скетче.
#include <AlashCompass.h>

AlashCompass compass;

void setup() {
  Serial.begin(115200);
  if (!compass.begin()) {
    Serial.println(compass.errorText());
    while (true) delay(1000);
  }
  Serial.println(F("Через 3 секунды начнём. Вращайте плату по кругу 15 секунд..."));
  delay(3000);
  if (compass.calibrate(15000)) {
    Serial.println(F("Готово. Вставьте эту строку в setup() после begin():"));
    compass.printCalibration(Serial);
  } else {
    Serial.println(compass.errorText());
  }
}

void loop() {
  Serial.println(compass.heading(), 0);   // после калибровки курс правильный на любом повороте
  delay(300);
}
