# AlashCompass

[Русский](#русский) | [Қазақша](#қазақша) | [English](#english)

---

## Русский

Простая библиотека для магнитного компаса **QMC5883L, HMC5883L и QMC5883P** (модуль **GY-271**): курс 0-360 градусов, сторона света словами («Север», «Юго-запад») и калибровка вращением платы. Сама определяет, какая микросхема стоит на плате (на GY-271 бывают обе). Только встроенная `Wire`. Сделана для уроков, роботов и наборов Alash Electronics.

```cpp
#include <AlashCompass.h>
AlashCompass compass;

void setup() { Serial.begin(115200); compass.begin(); }
void loop() {
  Serial.println(compass.heading(), 0);   // 0 = север / north, 90 = восток / east
  Serial.println(compass.direction());    // "Север" / "North"
  delay(200);
}
```

### Что умеет
- **Любая плата GY-271:** старая на QMC5883L (0x0D), на HMC5883L (0x1E) и новая на **QMC5883P (0x2C)** определяются сами.
- **Сама определяет чип:** QMC5883L (0x0D) или HMC5883L (0x1E). Порядок осей и масштаб учтены, пользователь получает одинаковые значения.
- **Курс и стороны света:** `heading()` 0-360, `direction()` словами и `directionShort()` (С, СВ, В...) на русском, казахском и английском.
- **Калибровка вращением:** `calibrate()` находит смещение и растяжение осей от железа рядом; `printCalibration()` печатает готовую строку для вставки в скетч. В тесте ошибка курса падает с 39 до 0.03 градуса.
- **Склонение и поправки:** `setDeclination(5.5)`, `setHeadingOffset(90)`, `invertHeading()`.
- **Сглаживание** векторов (а не углов), поэтому нет скачков на переходе 359 к 0.
- **Предупреждает о помехах:** если поле Земли не 25-65 мкТл или датчик перегружен, `diagnose()` скажет, что рядом магнит или мотор.
- **Понятные подсказки** на трёх языках: `errorText()`, `diagnose(Serial)`.
- Работает на **Arduino AVR, ESP32, ESP8266**.

### Подключение
| Модуль GY-271 | Uno / Nano | ESP32 |
|---|---|---|
| VCC | 5 В (или 3.3 В) | 3.3 В |
| GND | GND | GND |
| SDA | A4 | GPIO21 |
| SCL | A5 | GPIO22 |

Держите модуль горизонтально и **далеко от моторов, динамиков, магнитов и силовых проводов**: они сильно искажают поле. Если курс «ползёт» при включении мотора, вынесите компас на стойку подальше.

### Чем отличается от других библиотек
Мы посмотрели QMC5883L (mprograms), QMC5883LCompass (MPrograms), HMC5883L (jarzebski) и Adafruit HMC5883 Unified:
- Эти библиотеки только под один чип; на плате GY-271 бывает любой, и новичок получает «не найден». У нас оба.
- Adafruit требует Adafruit Unified Sensor; у нас одна библиотека.
- Калибровка и стороны света словами есть не везде; у нас `calibrate()`, `direction()` и подсказки.

### Примеры
`Basic`, `Calibrate`, `Direction`, `Diagnose`, `ESP32_CustomPins`

### Тесты
Оба чипа с разным порядком байтов и осей, масштаб в мкТл, восемь сторон света на всех четырёх октантах, склонение и переход через 360, сглаживание, калибровка на имитации вращения с железом рядом (ошибка с 39 до 0.03 градуса), переполнение, чужая микросхема проверены без железа: `extras/test`. Версия 1.1.0: QMC5883P (определение по идентификатору 0x80, порядок записи при настройке, данные и масштаб 3750 единиц на гаусс, переполнение, чужая микросхема на 0x2C).

Подробное руководство: [docs/WIKI.md](docs/WIKI.md).

---

## Қазақша

**QMC5883L және HMC5883L** магнит компасына (**GY-271** модулі) арналған қарапайым кітапхана: 0-360 градус бағыты, жақ сөзбен («Солтүстік», «Оңтүстік-батыс») және тақтаны айналдырып калибрлеу. Тақтада қандай микросхема тұрғанын өзі анықтайды. Тек кірістірілген `Wire`.

```cpp
#include <AlashCompass.h>
AlashCompass compass;

void setup() { Serial.begin(115200); compass.begin(); }
void loop() {
  Serial.println(compass.heading(), 0);   // 0 = север / north, 90 = восток / east
  Serial.println(compass.direction());    // "Север" / "North"
  delay(200);
}
```

### Мүмкіндіктері
- **Кез келген GY-271 тақтасы:** ескі QMC5883L (0x0D), HMC5883L (0x1E) және жаңа **QMC5883P (0x2C)** өзі анықталады.
- **Чипті өзі анықтайды:** QMC5883L (0x0D) не HMC5883L (0x1E).
- **Бағыт және жақтар:** `heading()` 0-360, `direction()` сөзбен, `directionShort()` үш тілде.
- **Айналдыру арқылы калибрлеу:** `calibrate()`; `printCalibration()` дайын жолды шығарады.
- **Ауытқу және түзетулер:** `setDeclination(5.5)`, `setHeadingOffset(90)`, `invertHeading()`.
- **Векторларды тегістеу**, 359-дан 0-ге секіру жоқ.
- **Кедергі туралы ескертеді:** жер өрісі 25-65 мкТл емес не сенсор шамадан тыс жүктелсе.
- **Түсінікті кеңестер** үш тілде.
- **Arduino AVR, ESP32, ESP8266**.

### Қосу
| GY-271 модулі | Uno / Nano | ESP32 |
|---|---|---|
| VCC | 5 В (или 3.3 В) | 3.3 В |
| GND | GND | GND |
| SDA | A4 | GPIO21 |
| SCL | A5 | GPIO22 |

Модульді көлденең ұстаңыз және **мотор, динамик, магнит пен күш сымдарынан алыс** қойыңыз.

### Басқа кітапханалардан айырмашылығы
QMC5883L (mprograms), QMC5883LCompass, HMC5883L (jarzebski) және Adafruit HMC5883 Unified қарадық:
- Олар бір чипке арналған; GY-271-де кез келгені болады. Бізде екеуі де.
- Adafruit-қа Unified Sensor керек; бізде бір кітапхана.
- Бізде `calibrate()`, `direction()` және кеңестер бар.

### Мысалдар
`Basic`, `Calibrate`, `Direction`, `Diagnose`, `ESP32_CustomPins`

### Тесттер
Екі чип, масштаб, сегіз жақ, ауытқу, тегістеу, айналдыру арқылы калибрлеу, толу, бөтен микросхема тексерілген: `extras/test`. 1.1.0 нұсқасы: QMC5883P (0x80 идентификаторы бойынша анықтау, баптау реті, деректер, толып кету).

Толық нұсқаулық: [docs/WIKI.md](docs/WIKI.md).

---

## English

A simple library for the **QMC5883L, HMC5883L and QMC5883P** magnetic compass (**GY-271** module): heading 0-360 degrees, the direction in words ("North", "South-west") and calibration by rotating the board. It tells which chip is on the board (GY-271 boards come with either). Only the built-in `Wire`. Made for classes, robots and Alash Electronics kits.

```cpp
#include <AlashCompass.h>
AlashCompass compass;

void setup() { Serial.begin(115200); compass.begin(); }
void loop() {
  Serial.println(compass.heading(), 0);   // 0 = север / north, 90 = восток / east
  Serial.println(compass.direction());    // "Север" / "North"
  delay(200);
}
```

### Features
- **Any GY-271 board:** the old ones on a QMC5883L (0x0D), an HMC5883L (0x1E) and the new ones on a **QMC5883P (0x2C)** are detected automatically.
- **Detects the chip itself:** QMC5883L (0x0D) or HMC5883L (0x1E). Axis order and scale are handled, so the user gets identical values.
- **Heading and directions:** `heading()` 0-360, `direction()` in words and `directionShort()` (N, NE, E...) in Russian, Kazakh and English.
- **Calibration by rotation:** `calibrate()` finds the offset and axis stretch caused by nearby iron; `printCalibration()` prints a line to paste into your sketch. In the test the heading error drops from 39 to 0.03 degrees.
- **Declination and corrections:** `setDeclination(5.5)`, `setHeadingOffset(90)`, `invertHeading()`.
- **Smoothing** of vectors (not angles), so no jump at the 359 to 0 wrap.
- **Warns about interference:** if the field is not 25-65 uT or the sensor saturates, `diagnose()` says a magnet or motor is nearby.
- **Clear tips** in three languages: `errorText()`, `diagnose(Serial)`.
- Works on **Arduino AVR, ESP32, ESP8266**.

### Wiring
| GY-271 module | Uno / Nano | ESP32 |
|---|---|---|
| VCC | 5 В (или 3.3 В) | 3.3 В |
| GND | GND | GND |
| SDA | A4 | GPIO21 |
| SCL | A5 | GPIO22 |

Keep the module level and **away from motors, speakers, magnets and power wires**: they distort the field strongly. If the heading drifts when a motor starts, move the compass away on a post.

### How it differs from other libraries
We looked at QMC5883L (mprograms), QMC5883LCompass, HMC5883L (jarzebski) and Adafruit HMC5883 Unified:
- They support only one chip, while a GY-271 board may carry either, so beginners get "not found". We support both.
- Adafruit needs Adafruit Unified Sensor; we are a single library.
- Calibration and directions in words are not everywhere; we have `calibrate()`, `direction()` and tips.

### Examples
`Basic`, `Calibrate`, `Direction`, `Diagnose`, `ESP32_CustomPins`

### Tests
Both chips with different byte and axis order, scale in uT, eight directions in all octants, declination and 360 wrap, smoothing, calibration on a simulated rotation with iron nearby (error from 39 to 0.03 degrees), overflow and a foreign chip are checked without hardware: `extras/test`. Version 1.1.0: QMC5883P (detection by the 0x80 identifier, the configuration write order, data and the 3750 LSB per gauss scale, overflow, a foreign chip at 0x2C).

Full guide: [docs/WIKI.md](docs/WIKI.md).

---

MIT License, Alash Electronics.
