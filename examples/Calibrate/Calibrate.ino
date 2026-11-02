// [ru] AlashCompass: калибровка. Рядом с датчиком всегда есть железо и провода, и без калибровки курс врёт на 10-40 градусов.
//      Когда сообщение появится, плавно вращайте плату по кругу в горизонтальной плоскости и немного наклоняйте её (15 секунд).
//      В конце библиотека напечатает строку setCalibration(...). Вставьте её в setup() после begin(): калибровка сохранится в скетче.
// [kk] AlashCompass: калибрлеу. Датчик жанында әрқашан темір мен сым болады, калибрлеусіз курс 10-40 градусқа қателеседі.
//      Хабарлама шыққанда тақтаны көлденең жазықтықта шеңбер бойымен жайлап айналдырып, сәл еңкейтіңіз (15 секунд).
//      Соңында кітапхана setCalibration(...) жолын басып шығарады. Оны begin()-нен кейін setup() ішіне қойыңыз: калибрлеу скетчте сақталады.
// [en] AlashCompass: calibration. There is always iron and wire near the sensor, and without calibration the heading is off by 10-40 degrees.
//      When the message appears, smoothly rotate the board in a circle in a horizontal plane and tilt it a little (15 seconds).
//      At the end the library prints a setCalibration(...) line. Paste it into setup() after begin(): the calibration is stored in the sketch.
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
