// MIT License. Alash Electronics.
#include "AlashCompass.h"

#define T(ru, kk, en) (_lang == LANG_KK ? F(kk) : _lang == LANG_EN ? F(en) : F(ru))

static const float RAD2DEG = 57.2957795f;

// ---- Запуск --------------------------------------------------------------------

bool AlashCompass::begin(TwoWire &wire, bool startWire) {
  _wire = &wire;
  if (startWire) _wire->begin();
  _chip = CHIP_NONE;
  _have = false;
  return probe();
}

#if defined(ESP32) || defined(ESP8266)
bool AlashCompass::begin(int sdaPin, int sclPin) {
  Wire.begin(sdaPin, sclPin);
  return begin(Wire, false);
}
#endif

bool AlashCompass::probe() {
  Error first = ERR_NO_DEVICE;
  // QMC5883L: адрес 0x0D, регистр 0x0D содержит 0xFF
  _addr = 0x0D;
  _wire->beginTransmission(_addr);
  if (_wire->endTransmission() == 0) {
    uint8_t id = 0;
    if (readBytes(0x0D, &id, 1) && id == 0xFF) {
      write8(0x0A, 0x80);                    // программный сброс
      delay(5);
      write8(0x0B, 0x01);                    // период SET/RESET по рекомендации производителя
      // OSR 512, диапазон ±8 Гс, 200 Гц, непрерывный режим
      if (!write8(0x09, (0 << 6) | (1 << 4) | (3 << 2) | 1)) {
        _err = ERR_I2C;
        return false;
      }
      _chip = CHIP_QMC5883L;
      _err = ERR_OK;
      return true;
    }
    first = ERR_WRONG_CHIP;
  }
  // HMC5883L: адрес 0x1E, идентификатор "H43" в регистрах 0x0A-0x0C
  _addr = 0x1E;
  _wire->beginTransmission(_addr);
  if (_wire->endTransmission() == 0) {
    uint8_t id[3] = {0, 0, 0};
    if (readBytes(0x0A, id, 3) && id[0] == 'H' && id[1] == '4' && id[2] == '3') {
      if (!write8(0x00, 0x70) || !write8(0x01, 0x20) || !write8(0x02, 0x00)) {   // усреднение 8, 15 Гц; усиление ±1.3 Гс; непрерывно
        _err = ERR_I2C;
        return false;
      }
      _chip = CHIP_HMC5883L;
      _err = ERR_OK;
      return true;
    }
    first = ERR_WRONG_CHIP;
  }
  // Новый чип QMC5883P на адресе 0x2C на платах GY-271 тоже встречается; он здесь не поддерживается
  _addr = 0x2C;
  _wire->beginTransmission(_addr);
  if (_wire->endTransmission() == 0) first = ERR_WRONG_CHIP;
  _err = first;
  return false;
}

bool AlashCompass::isConnected() {
  _wire->beginTransmission(_addr);
  return _wire->endTransmission() == 0;
}

const __FlashStringHelper *AlashCompass::chipName() const {
  return _chip == CHIP_QMC5883L ? F("QMC5883L") : _chip == CHIP_HMC5883L ? F("HMC5883L") : F("-");
}

// ---- I2C -------------------------------------------------------------------------

bool AlashCompass::readBytes(uint8_t reg, uint8_t *buf, uint8_t len) {
  _wire->beginTransmission(_addr);
  _wire->write(reg);
  if (_wire->endTransmission(false) != 0) return false;
  if (_wire->requestFrom(_addr, len) != len) return false;
  for (uint8_t i = 0; i < len; i++) buf[i] = (uint8_t)_wire->read();
  return true;
}

bool AlashCompass::write8(uint8_t reg, uint8_t v) {
  _wire->beginTransmission(_addr);
  _wire->write(reg);
  _wire->write(v);
  return _wire->endTransmission() == 0;
}

// Одно измерение в микротеслах. Порядок данных у чипов разный: QMC X,Y,Z (младший байт первый); HMC X,Z,Y (старший первый).
bool AlashCompass::sample(float *v) {
  if (_chip == CHIP_NONE) {
    _err = ERR_NO_DEVICE;
    return false;
  }
  uint8_t b[6], st = 0;
  for (uint8_t tries = 0; tries < 20; tries++) {          // ждём готовности данных до ~20 мс
    uint8_t reg = _chip == CHIP_QMC5883L ? 0x06 : 0x09;
    if (!readBytes(reg, &st, 1)) {
      _err = ERR_I2C;
      return false;
    }
    if (st & 0x01) break;
    delay(1);
  }
  if (!readBytes(_chip == CHIP_QMC5883L ? 0x00 : 0x03, b, 6)) {
    _err = ERR_I2C;
    return false;
  }
  int16_t rx, ry, rz;
  float perLsb;                                             // мкТл на единицу
  if (_chip == CHIP_QMC5883L) {
    rx = (int16_t)(b[1] << 8 | b[0]);
    ry = (int16_t)(b[3] << 8 | b[2]);
    rz = (int16_t)(b[5] << 8 | b[4]);
    perLsb = 100.0f / 3000.0f;                              // ±8 Гс: 3000 единиц на гаусс, 1 Гс = 100 мкТл
    if (st & 0x02) {                                         // OVL: сигнал за пределами диапазона
      _err = ERR_OVERFLOW;
      return false;
    }
  } else {
    rx = (int16_t)(b[0] << 8 | b[1]);
    rz = (int16_t)(b[2] << 8 | b[3]);
    ry = (int16_t)(b[4] << 8 | b[5]);
    perLsb = 100.0f / 1090.0f;                              // ±1.3 Гс: 1090 единиц на гаусс
    if (rx == -4096 || ry == -4096 || rz == -4096) {         // -4096 это признак переполнения HMC5883L
      _err = ERR_OVERFLOW;
      return false;
    }
  }
  v[0] = rx * perLsb;
  v[1] = ry * perLsb;
  v[2] = rz * perLsb;
  _err = ERR_OK;
  return true;
}

bool AlashCompass::update() {
  float v[3];
  if (!sample(v)) return false;
  for (uint8_t i = 0; i < 3; i++) {
    _raw[i] = v[i];
    // скользящее среднее по векторам (а не по углу: усреднять углы нельзя из-за перехода 359 -> 0)
    _avg[i] = _have ? _avg[i] + (v[i] - _avg[i]) / _smooth : v[i];
  }
  _have = true;
  return true;
}

void AlashCompass::ensureFresh() { update(); }

float AlashCompass::x() { ensureFresh(); return _have ? (_avg[0] - _off[0]) * _scl[0] : NAN; }
float AlashCompass::y() { ensureFresh(); return _have ? (_avg[1] - _off[1]) * _scl[1] : NAN; }
float AlashCompass::z() { ensureFresh(); return _have ? (_avg[2] - _off[2]) * _scl[2] : NAN; }

float AlashCompass::fieldStrength() {
  ensureFresh();
  if (!_have) return NAN;
  float a = (_avg[0] - _off[0]) * _scl[0], b = (_avg[1] - _off[1]) * _scl[1], c = (_avg[2] - _off[2]) * _scl[2];
  return sqrtf(a * a + b * b + c * c);
}

// ---- Курс ---------------------------------------------------------------------------

float AlashCompass::heading() {
  ensureFresh();
  if (!_have || _err == ERR_OVERFLOW) return NAN;
  float a = (_avg[0] - _off[0]) * _scl[0], b = (_avg[1] - _off[1]) * _scl[1];
  float h = atan2f(b, a) * RAD2DEG;
  if (_invert) h = -h;
  h += _offset + _decl;
  while (h < 0) h += 360.0f;
  while (h >= 360.0f) h -= 360.0f;
  return h;
}

const __FlashStringHelper *AlashCompass::direction() {
  float h = heading();
  if (isnan(h)) return F("?");
  uint8_t i = (uint8_t)((h + 22.5f) / 45.0f) % 8;           // 0 = север, 1 = северо-восток ...
  switch (i) {
    case 0: return T("Север", "Солтүстік", "North");
    case 1: return T("Северо-восток", "Солтүстік-шығыс", "North-east");
    case 2: return T("Восток", "Шығыс", "East");
    case 3: return T("Юго-восток", "Оңтүстік-шығыс", "South-east");
    case 4: return T("Юг", "Оңтүстік", "South");
    case 5: return T("Юго-запад", "Оңтүстік-батыс", "South-west");
    case 6: return T("Запад", "Батыс", "West");
    default: return T("Северо-запад", "Солтүстік-батыс", "North-west");
  }
}

const __FlashStringHelper *AlashCompass::directionShort() {
  float h = heading();
  if (isnan(h)) return F("?");
  uint8_t i = (uint8_t)((h + 22.5f) / 45.0f) % 8;
  if (_lang == LANG_EN) {
    static const char *en[8] = {"N", "NE", "E", "SE", "S", "SW", "W", "NW"};
    return (const __FlashStringHelper *)en[i];
  }
  if (_lang == LANG_KK) {
    static const char *kk[8] = {"С", "СШ", "Ш", "ОШ", "О", "ОБ", "Б", "СБ"};
    return (const __FlashStringHelper *)kk[i];
  }
  static const char *ru[8] = {"С", "СВ", "В", "ЮВ", "Ю", "ЮЗ", "З", "СЗ"};
  return (const __FlashStringHelper *)ru[i];
}

// ---- Калибровка ---------------------------------------------------------------------------
// Рядом с датчиком всегда есть железо и провода: они сдвигают центр круга (hard iron) и сплющивают его в эллипс (soft iron).
// Крутим плату, запоминаем минимумы и максимумы по осям: центр = (мин + макс) / 2, растяжение выравнивает размах осей.

bool AlashCompass::calibrate(uint16_t durationMs) {
  if (_chip == CHIP_NONE) {
    _err = ERR_NO_DEVICE;
    return false;
  }
  float mn[3] = {1e9f, 1e9f, 1e9f}, mx[3] = {-1e9f, -1e9f, -1e9f};
  uint32_t t0 = millis();
  uint16_t n = 0;
  while ((uint32_t)(millis() - t0) < durationMs || n < 5) {
    float v[3];
    if (sample(v)) {
      for (uint8_t i = 0; i < 3; i++) {
        if (v[i] < mn[i]) mn[i] = v[i];
        if (v[i] > mx[i]) mx[i] = v[i];
      }
      n++;
    } else if (_err != ERR_OVERFLOW) {
      return false;
    }
    delay(10);
  }
  // Платы вращали только в горизонтальной плоскости? Тогда ось Z почти не меняется: её растяжение не трогаем
  float span[3];
  for (uint8_t i = 0; i < 3; i++) span[i] = mx[i] - mn[i];
  if (span[0] < 20.0f || span[1] < 20.0f) {                 // для круга нужен размах хотя бы 20 мкТл по X и Y (поле Земли 25-65 мкТл)
    _err = ERR_NOT_ROTATED;
    return false;
  }
  for (uint8_t i = 0; i < 3; i++) _off[i] = (mx[i] + mn[i]) / 2.0f;
  float avg = (span[0] + span[1]) / 2.0f;
  _scl[0] = avg / span[0];
  _scl[1] = avg / span[1];
  _scl[2] = span[2] > 20.0f ? (avg / span[2] > 3 ? 1.0f : avg / span[2]) : 1.0f;
  _have = false;
  _err = ERR_OK;
  return true;
}

void AlashCompass::setCalibration(float ox, float oy, float oz, float sx, float sy, float sz) {
  _off[0] = ox; _off[1] = oy; _off[2] = oz;
  _scl[0] = sx; _scl[1] = sy; _scl[2] = sz;
}

void AlashCompass::printCalibration(Print &out) {
  out.print(F("compass.setCalibration("));
  for (uint8_t i = 0; i < 3; i++) { out.print(_off[i], 2); out.print(F(", ")); }
  for (uint8_t i = 0; i < 3; i++) { out.print(_scl[i], 3); if (i < 2) out.print(F(", ")); }
  out.println(F(");"));
}

// ---- Диагностика ------------------------------------------------------------------------------

const __FlashStringHelper *AlashCompass::errorText() {
  switch (_err) {
    case ERR_OK: return F("OK");
    case ERR_NO_DEVICE:
      return T("Компас не найден: проверьте SDA / SCL, питание (3.3-5 В) и GND. Нужен QMC5883L (адрес 0x0D) или HMC5883L (0x1E)",
               "Компас табылмады: SDA / SCL, қуатты (3.3-5 В) және GND тексеріңіз. QMC5883L (0x0D) не HMC5883L (0x1E) керек",
               "Compass not found: check SDA / SCL, power (3.3-5 V) and GND. A QMC5883L (address 0x0D) or HMC5883L (0x1E) is required");
    case ERR_WRONG_CHIP:
      return T("На плате другая микросхема (возможно, QMC5883P на адресе 0x2C): эта библиотека её не поддерживает",
               "Тақтада басқа микросхема (мүмкін 0x2C мекенжайындағы QMC5883P): бұл кітапхана оны қолдамайды",
               "The board has another chip (possibly a QMC5883P at 0x2C): this library does not support it");
    case ERR_I2C:
      return T("Ошибка I2C: проверьте контакты и подтяжку линий", "I2C қатесі: байланысты және тартқыш резисторларды тексеріңіз",
               "I2C error: check connections and pull-up resistors");
    case ERR_OVERFLOW:
      return T("Поле слишком сильное: рядом магнит, динамик, мотор или реле. Уберите источник подальше от датчика",
               "Өріс тым күшті: жақын жерде магнит, динамик, мотор не реле. Көзді сенсордан алыс қойыңыз",
               "The field is too strong: a magnet, speaker, motor or relay is nearby. Move the source away from the sensor");
    case ERR_NOT_ROTATED:
      return T("Калибровка не удалась: плату надо плавно повернуть на полный круг (и немного наклонить) за время калибровки",
               "Калибрлеу сәтсіз: калибрлеу кезінде тақтаны толық шеңберге баяу бұрыңыз (және аздап еңкейтіңіз)",
               "Calibration failed: slowly turn the board through a full circle (and tilt it a little) during calibration");
  }
  return F("");
}

void AlashCompass::diagnose(Print &out) {
  out.println(T("AlashCompass: диагностика", "AlashCompass: диагностика", "AlashCompass: diagnostics"));
  out.println(T("Найденные устройства I2C:", "Табылған I2C құрылғылары:", "I2C devices found:"));
  for (uint8_t a = 1; a < 127; a++) {
    _wire->beginTransmission(a);
    if (_wire->endTransmission() == 0) {
      out.print(F("  0x"));
      if (a < 16) out.print('0');
      out.print(a, HEX);
      if (a == 0x0D) out.print(F("  QMC5883L"));
      else if (a == 0x1E) out.print(F("  HMC5883L"));
      else if (a == 0x2C) out.print(F("  QMC5883P (не поддерживается)"));
      else if (a == 0x68) out.print(F("  MPU6050 / DS3231"));
      out.println();
    }
  }
  if (_chip == CHIP_NONE) {
    out.println(errorText());
    return;
  }
  out.print(T("Микросхема: ", "Микросхема: ", "Chip: "));
  out.println(chipName());
  if (!update()) {
    out.println(errorText());
    return;
  }
  out.print(T("Поле X Y Z (мкТл): ", "Өріс X Y Z (мкТл): ", "Field X Y Z (uT): "));
  out.print(_raw[0], 1); out.print(F(" ")); out.print(_raw[1], 1); out.print(F(" ")); out.println(_raw[2], 1);
  float f = sqrtf(_raw[0] * _raw[0] + _raw[1] * _raw[1] + _raw[2] * _raw[2]);
  out.print(T("Величина: ", "Шамасы: ", "Magnitude: "));
  out.print(f, 1);
  out.println(F(" uT"));
  if (f < 15.0f || f > 80.0f)
    out.println(T("Поле Земли должно быть 25-65 мкТл. Здесь иначе: рядом железо, магниты, провода с током или мотор, либо не тот чип. Уберите их, затем вызовите calibrate().",
                  "Жер өрісі 25-65 мкТл болуы керек. Мұнда басқаша: жақын жерде темір, магнит, тоғы бар сым не мотор. Оларды алып, calibrate() шақырыңыз.",
                  "Earth's field should be 25-65 uT. Here it is not: iron, magnets, current-carrying wires or a motor are close. Remove them, then call calibrate()."));
  out.println(T("Проверка направления: плавно поворачивайте плату по часовой стрелке (вид сверху): курс должен расти. Если убывает, вызовите invertHeading(). Если север показан в другой стороне на постоянный угол, setHeadingOffset(угол).",
                "Бағытты тексеру: тақтаны сағат тілі бойынша баяу бұрыңыз (жоғарыдан қарағанда): бағыт өсуі керек. Кемісе, invertHeading() шақырыңыз. Солтүстік тұрақты бұрышқа ауытқыса, setHeadingOffset(бұрыш).",
                "Direction check: slowly turn the board clockwise (seen from above): the heading must grow. If it decreases, call invertHeading(). If north is off by a constant angle, use setHeadingOffset(angle)."));
}
