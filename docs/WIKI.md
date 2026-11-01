# AlashCompass: руководство / нұсқаулық / guide

[Русский](#русский) | [Қазақша](#қазақша) | [English](#english)

---

## Русский

Простая библиотека для магнитного компаса **QMC5883L и HMC5883L** (модуль **GY-271**): курс 0-360 градусов, сторона света словами («Север», «Юго-запад») и калибровка вращением платы. Сама определяет, какая микросхема стоит на плате (на GY-271 бывают обе). Только встроенная `Wire`. Сделана для уроков, роботов и наборов Alash Electronics.

### Установка
Arduino IDE: Скетч, Подключить библиотеку, Управлять библиотеками, найти **AlashCompass** и нажать Установить. Или скачайте ZIP с GitHub: Скетч, Подключить библиотеку, Добавить .ZIP библиотеку.

### Платы
Arduino Uno / Nano / Mega (AVR), ESP32, ESP8266 и другие, где работает Arduino-ядро.

### Подключение
| Модуль GY-271 | Uno / Nano | ESP32 |
|---|---|---|
| VCC | 5 В (или 3.3 В) | 3.3 В |
| GND | GND | GND |
| SDA | A4 | GPIO21 |
| SCL | A5 | GPIO22 |

Держите модуль горизонтально и **далеко от моторов, динамиков, магнитов и силовых проводов**: они сильно искажают поле. Если курс «ползёт» при включении мотора, вынесите компас на стойку подальше.

### Быстрый старт
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
- **Сама определяет чип:** QMC5883L (0x0D) или HMC5883L (0x1E). Порядок осей и масштаб учтены, пользователь получает одинаковые значения.
- **Курс и стороны света:** `heading()` 0-360, `direction()` словами и `directionShort()` (С, СВ, В...) на русском, казахском и английском.
- **Калибровка вращением:** `calibrate()` находит смещение и растяжение осей от железа рядом; `printCalibration()` печатает готовую строку для вставки в скетч. В тесте ошибка курса падает с 39 до 0.03 градуса.
- **Склонение и поправки:** `setDeclination(5.5)`, `setHeadingOffset(90)`, `invertHeading()`.
- **Сглаживание** векторов (а не углов), поэтому нет скачков на переходе 359 к 0.
- **Предупреждает о помехах:** если поле Земли не 25-65 мкТл или датчик перегружен, `diagnose()` скажет, что рядом магнит или мотор.
- **Понятные подсказки** на трёх языках: `errorText()`, `diagnose(Serial)`.
- Работает на **Arduino AVR, ESP32, ESP8266**.

### Справочник по методам
| Метод | Что делает |
|---|---|
| `begin()` | Найти и настроить компас (QMC или HMC) |
| `heading()` | Курс, градусы 0..360 (0 = север) |
| `direction() / directionShort()` | Сторона света словами / коротко |
| `setDeclination(deg)` | Магнитное склонение места |
| `setHeadingOffset(deg) / invertHeading()` | Поправка при повороте платы / обратное направление |
| `calibrate(ms)` | Калибровка вращением платы |
| `setCalibration(...) / printCalibration(Serial)` | Задать / напечатать калибровку |
| `x() y() z() / fieldStrength()` | Поле, мкТл / величина |
| `setSmoothing(n)` | Усреднение по n измерениям |
| `diagnose(Serial)` | Самопроверка и подсказки |

### Если что-то не работает
**Курс врёт на десятки градусов**

Нужна калибровка: запустите пример Calibrate и вращайте плату. Уберите от датчика моторы, динамики, магниты, батареи и толстые провода с током.

**При повороте по часовой курс уменьшается**

Вызовите `compass.invertHeading()`. Если север показан со сдвигом на постоянный угол, `setHeadingOffset(угол)`.

**Компас не найден, хотя плата подключена**

Выполните пример Diagnose. Новые платы GY-271 бывают с чипом QMC5883P (адрес 0x2C): он не поддерживается этой библиотекой.

### Самопроверка
В библиотеке есть `diagnose(Serial)`: она печатает, что видит плата, и подсказывает, что проверить. Язык подсказок: `setLanguage(LANG_RU / LANG_KK / LANG_EN)`.

### Примеры
`Basic`, `Calibrate`, `Direction`, `Diagnose`, `ESP32_CustomPins`

---

## Қазақша

**QMC5883L және HMC5883L** магнит компасына (**GY-271** модулі) арналған қарапайым кітапхана: 0-360 градус бағыты, жақ сөзбен («Солтүстік», «Оңтүстік-батыс») және тақтаны айналдырып калибрлеу. Тақтада қандай микросхема тұрғанын өзі анықтайды. Тек кірістірілген `Wire`.

### Орнату
Arduino IDE: Sketch, Include Library, Manage Libraries, **AlashCompass** табып, Install басыңыз. Немесе GitHub-тан ZIP жүктеп, Sketch, Include Library, Add .ZIP Library.

### Тақталар
Arduino Uno / Nano / Mega (AVR), ESP32, ESP8266 және Arduino ядросы бар басқалар.

### Қосу
| GY-271 модулі | Uno / Nano | ESP32 |
|---|---|---|
| VCC | 5 В (или 3.3 В) | 3.3 В |
| GND | GND | GND |
| SDA | A4 | GPIO21 |
| SCL | A5 | GPIO22 |

Модульді көлденең ұстаңыз және **мотор, динамик, магнит пен күш сымдарынан алыс** қойыңыз.

### Жылдам бастау
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
- **Чипті өзі анықтайды:** QMC5883L (0x0D) не HMC5883L (0x1E).
- **Бағыт және жақтар:** `heading()` 0-360, `direction()` сөзбен, `directionShort()` үш тілде.
- **Айналдыру арқылы калибрлеу:** `calibrate()`; `printCalibration()` дайын жолды шығарады.
- **Ауытқу және түзетулер:** `setDeclination(5.5)`, `setHeadingOffset(90)`, `invertHeading()`.
- **Векторларды тегістеу**, 359-дан 0-ге секіру жоқ.
- **Кедергі туралы ескертеді:** жер өрісі 25-65 мкТл емес не сенсор шамадан тыс жүктелсе.
- **Түсінікті кеңестер** үш тілде.
- **Arduino AVR, ESP32, ESP8266**.

### Әдістер анықтамасы
| Әдіс | Не істейді |
|---|---|
| `begin()` | Компасты табу және баптау |
| `heading()` | Бағыт, градус |
| `direction() / directionShort()` | Жақ сөзбен / қысқа |
| `setDeclination(deg)` | Магнит ауытқуы |
| `setHeadingOffset(deg) / invertHeading()` | Түзету / кері бағыт |
| `calibrate(ms)` | Тақтаны айналдырып калибрлеу |
| `setCalibration(...) / printCalibration(Serial)` | Калибрлеуді беру / шығару |
| `x() y() z() / fieldStrength()` | Өріс, мкТл / шамасы |
| `setSmoothing(n)` | Орташалау |
| `diagnose(Serial)` | Өзін тексеру |

### Бірдеңе жұмыс істемесе
**Бағыт ондаған градусқа қателеседі**

Калибрлеу керек: Calibrate мысалын іске қосыңыз. Моторлар мен магниттерді алыс қойыңыз.

**Сағат тілімен бұрғанда бағыт кемиді**

`compass.invertHeading()` шақырыңыз.

**Компас табылмады**

Diagnose мысалын іске қосыңыз. Жаңа GY-271 тақталарында QMC5883P (0x2C) болады: қолдау жоқ.

### Өзін-өзі тексеру
Кітапханада `diagnose(Serial)` бар: тақта не көргенін басып шығарады және нені тексеру керегін айтады. Кеңес тілі: `setLanguage(LANG_RU / LANG_KK / LANG_EN)`.

### Мысалдар
`Basic`, `Calibrate`, `Direction`, `Diagnose`, `ESP32_CustomPins`

---

## English

A simple library for the **QMC5883L and HMC5883L** magnetic compass (**GY-271** module): heading 0-360 degrees, the direction in words ("North", "South-west") and calibration by rotating the board. It tells which chip is on the board (GY-271 boards come with either). Only the built-in `Wire`. Made for classes, robots and Alash Electronics kits.

### Installation
Arduino IDE: Sketch, Include Library, Manage Libraries, search **AlashCompass** and click Install. Or download the ZIP from GitHub: Sketch, Include Library, Add .ZIP Library.

### Boards
Arduino Uno / Nano / Mega (AVR), ESP32, ESP8266 and others with an Arduino core.

### Wiring
| GY-271 module | Uno / Nano | ESP32 |
|---|---|---|
| VCC | 5 В (или 3.3 В) | 3.3 В |
| GND | GND | GND |
| SDA | A4 | GPIO21 |
| SCL | A5 | GPIO22 |

Keep the module level and **away from motors, speakers, magnets and power wires**: they distort the field strongly. If the heading drifts when a motor starts, move the compass away on a post.

### Quick start
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
- **Detects the chip itself:** QMC5883L (0x0D) or HMC5883L (0x1E). Axis order and scale are handled, so the user gets identical values.
- **Heading and directions:** `heading()` 0-360, `direction()` in words and `directionShort()` (N, NE, E...) in Russian, Kazakh and English.
- **Calibration by rotation:** `calibrate()` finds the offset and axis stretch caused by nearby iron; `printCalibration()` prints a line to paste into your sketch. In the test the heading error drops from 39 to 0.03 degrees.
- **Declination and corrections:** `setDeclination(5.5)`, `setHeadingOffset(90)`, `invertHeading()`.
- **Smoothing** of vectors (not angles), so no jump at the 359 to 0 wrap.
- **Warns about interference:** if the field is not 25-65 uT or the sensor saturates, `diagnose()` says a magnet or motor is nearby.
- **Clear tips** in three languages: `errorText()`, `diagnose(Serial)`.
- Works on **Arduino AVR, ESP32, ESP8266**.

### Method reference
| Method | What it does |
|---|---|
| `begin()` | Find and configure the compass (QMC or HMC) |
| `heading()` | Heading, degrees 0..360 (0 = north) |
| `direction() / directionShort()` | Direction in words / short |
| `setDeclination(deg)` | Magnetic declination |
| `setHeadingOffset(deg) / invertHeading()` | Offset for a rotated board / reverse direction |
| `calibrate(ms)` | Calibration by rotating the board |
| `setCalibration(...) / printCalibration(Serial)` | Set / print the calibration |
| `x() y() z() / fieldStrength()` | Field, uT / magnitude |
| `setSmoothing(n)` | Average over n readings |
| `diagnose(Serial)` | Self-check and tips |

### If something does not work
**The heading is off by tens of degrees**

Calibrate: run the Calibrate example and rotate the board. Keep motors, speakers, magnets, batteries and thick current-carrying wires away.

**The heading decreases when turning clockwise**

Call `compass.invertHeading()`. If north is off by a constant angle, use `setHeadingOffset(angle)`.

**The compass is not found though connected**

Run the Diagnose example. Newer GY-271 boards carry a QMC5883P (address 0x2C): not supported by this library.

### Self-check
The library has `diagnose(Serial)`: it prints what the board sees and tells what to check. Tips language: `setLanguage(LANG_RU / LANG_KK / LANG_EN)`.

### Examples
`Basic`, `Calibrate`, `Direction`, `Diagnose`, `ESP32_CustomPins`

---

MIT License, Alash Electronics. https://github.com/Alash-electronics/AlashCompass
