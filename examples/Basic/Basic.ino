// [ru] AlashCompass: самый простой пример. Курс в градусах и сторона света с модуля GY-271 (QMC5883L или HMC5883L).
//
//      Подключение: VCC к 3.3 В или 5 В, GND к GND, SDA к A4 (ESP32: GPIO21), SCL к A5 (ESP32: GPIO22).
//      Держите плату горизонтально, подальше от моторов, магнитов и проводов с большим током.
// [kk] AlashCompass: ең қарапайым мысал. GY-271 модулінен (QMC5883L немесе HMC5883L) градуспен курс және әлем тарапы.
//
//      Қосу: VCC 3.3 В немесе 5 В-қа, GND GND-ға, SDA A4-ке (ESP32: GPIO21), SCL A5-ке (ESP32: GPIO22).
//      Тақтаны көлденең ұстаңыз, моторлардан, магниттерден және үлкен тогы бар сымдардан алыс.
// [en] AlashCompass: the simplest example. The heading in degrees and the compass direction from a GY-271 module (QMC5883L or HMC5883L).
//
//      Wiring: VCC to 3.3 V or 5 V, GND to GND, SDA to A4 (ESP32: GPIO21), SCL to A5 (ESP32: GPIO22).
//      Keep the board level, away from motors, magnets and wires with large current.
#include <AlashCompass.h>

AlashCompass compass;

void setup() {
  Serial.begin(115200);
  if (!compass.begin()) {                 // сам определяет микросхему на плате
    Serial.println(compass.errorText());  // объяснит, что проверить
    while (true) delay(1000);
  }
  // compass.setDeclination(5.5);         // магнитное склонение вашего города для точного севера (см. NOAA)
}

void loop() {
  Serial.print(compass.heading(), 0);     // 0 = север, 90 = восток, 180 = юг, 270 = запад
  Serial.print(F(" градусов  "));
  Serial.println(compass.direction());    // "Север", "Северо-восток", ...
  delay(200);
}
