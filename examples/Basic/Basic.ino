// AlashCompass: самый простой пример. Курс в градусах и сторона света с модуля GY-271 (QMC5883L или HMC5883L).
//
// Подключение: VCC к 3.3 В или 5 В, GND к GND, SDA к A4 (ESP32: GPIO21), SCL к A5 (ESP32: GPIO22).
// Держите плату горизонтально, подальше от моторов, магнитов и проводов с большим током.
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
