// AlashCompass: магнитный компас QMC5883L, HMC5883L и новый QMC5883P (модуль GY-271 / GY-273): курс 0-360 градусов, стороны света, калибровка вращением.
// Без зависимостей, только встроенная Wire. MIT License. Alash Electronics, https://github.com/Alash-electronics/AlashCompass
#ifndef ALASH_COMPASS_H
#define ALASH_COMPASS_H

#include <Arduino.h>
#include <Wire.h>

class AlashCompass {
 public:
  enum Chip : uint8_t { CHIP_NONE = 0, CHIP_QMC5883L, CHIP_HMC5883L, CHIP_QMC5883P };
  enum Error : uint8_t {
    ERR_OK = 0,
    ERR_NO_DEVICE,     // на шине I2C нет компаса (адреса 0x0D, 0x1E и 0x2C)
    ERR_WRONG_CHIP,    // по адресу компаса отвечает другая, неизвестная микросхема
    ERR_I2C,           // ошибка чтения или записи по I2C
    ERR_OVERFLOW,      // поле слишком сильное для датчика: рядом магнит или мотор
    ERR_NOT_ROTATED    // при калибровке датчик не повернули на полный круг
  };
  enum Language : uint8_t { LANG_RU = 0, LANG_KK = 1, LANG_EN = 2 };

  // ---- Запуск --------------------------------------------------------------
  // begin() сам определяет микросхему на плате GY-271: QMC5883L (адрес 0x0D), HMC5883L (0x1E) или новый QMC5883P (0x2C).
  bool begin(TwoWire &wire = Wire, bool startWire = true);
#if defined(ESP32) || defined(ESP8266)
  bool begin(int sdaPin, int sclPin);
#endif

  // ---- Курс ---------------------------------------------------------------------
  float heading();                          // курс, градусы 0..360 (0 = север, 90 = восток); NAN при ошибке
  const __FlashStringHelper *direction();   // сторона света словами: "Север", "Северо-восток"... на выбранном языке
  const __FlashStringHelper *directionShort();   // коротко: "С", "СВ", "В"... (N, NE, E для английского)
  void setDeclination(float degrees) { _decl = degrees; }   // магнитное склонение места (восточное плюс)
  void setHeadingOffset(float degrees) { _offset = degrees; }   // если плата повёрнута, например на 90 градусов
  void invertHeading(bool on = true) { _invert = on; }     // если при повороте по часовой курс уменьшается
  void setSmoothing(uint8_t n) { _smooth = n < 1 ? 1 : n > 16 ? 16 : n; }   // усреднение по n измерениям (по умолчанию 4)

  // ---- Поле (микротесла) ---------------------------------------------------------
  float x();                                // составляющие магнитного поля с учётом калибровки, мкТл
  float y();
  float z();
  float fieldStrength();                    // полная величина, мкТл (поле Земли 25-65 мкТл)
  bool update();

  // ---- Калибровка ------------------------------------------------------------------
  // Вращайте плату в горизонтальной плоскости (и немного наклоняйте) durationMs. Находит смещение и растяжение осей от железа рядом.
  bool calibrate(uint16_t durationMs = 15000);
  void setCalibration(float offX, float offY, float offZ, float scaleX = 1, float scaleY = 1, float scaleZ = 1);
  void printCalibration(Print &out);        // строка setCalibration(...) для вставки в скетч

  // ---- Состояние и диагностика --------------------------------------------------------
  Chip chip() const { return _chip; }
  const __FlashStringHelper *chipName() const;
  uint8_t address() const { return _addr; }
  bool isConnected();
  Error error() const { return _err; }
  void setLanguage(Language lang) { _lang = lang; }
  const __FlashStringHelper *errorText();
  void diagnose(Print &out);

 private:
  TwoWire *_wire = &Wire;
  uint8_t _addr = 0;
  Chip _chip = CHIP_NONE;
  Error _err = ERR_OK;
  Language _lang = LANG_RU;
  float _decl = 0, _offset = 0;
  bool _invert = false;
  uint8_t _smooth = 4;
  float _off[3] = {0, 0, 0}, _scl[3] = {1, 1, 1};
  float _raw[3] = {0, 0, 0};                // последние измерения без калибровки, мкТл
  float _avg[3] = {0, 0, 0};                // скользящее среднее
  bool _have = false;

  bool probe();
  bool readBytes(uint8_t reg, uint8_t *buf, uint8_t len);
  bool write8(uint8_t reg, uint8_t v);
  bool sample(float *v);                    // одно измерение, мкТл
  void ensureFresh();
};

#endif  // ALASH_COMPASS_H
