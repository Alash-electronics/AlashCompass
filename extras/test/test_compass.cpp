// Тесты AlashCompass без железа (имитация QMC5883L и HMC5883L, вращение платы для калибровки):
//   g++ -std=c++17 -Wall -I mock -I ../../src test_compass.cpp ../../src/AlashCompass.cpp && ./a.out
#include <initializer_list>
#include <stdio.h>
#include "Arduino.h"
#include "Wire.h"
#include "AlashCompass.h"

static int fails = 0;
#define CHECK(cond) do { if (!(cond)) { printf("FAIL line %d: %s\n", __LINE__, #cond); fails++; } } while (0)
static bool near(float a, float b, float e) { return fabsf(a - b) <= e; }
static float angDiff(float a, float b) { float d = fmodf(a - b + 540.0f, 360.0f) - 180.0f; return fabsf(d); }
static void reset(bool qmc = true) { g_cmp = FakeCompass(); g_cmp.qmc = qmc; g_cmp.addr = qmc ? 0x0D : 0x1E; g_onDelay = nullptr; g_ms += 1000; }
static void field(float x, float y, float z = 40) { g_cmp.fx = x; g_cmp.fy = y; g_cmp.fz = z; }

// Плата вращается в поле Земли 30 мкТл по горизонтали; рядом "железо": смещение и растяжение осей
static float spinAngle = 0;
static void spin() {
  spinAngle += 7.0f;
  float a = spinAngle * 3.14159265f / 180.0f;
  g_cmp.fx = 12.0f + 30.0f * 1.3f * cosf(a);         // hard iron +12, soft iron: ось X растянута на 30 %
  g_cmp.fy = -7.0f + 30.0f * 0.8f * sinf(a);         // hard iron -7, ось Y сжата до 80 %
  g_cmp.fz = 25.0f + 6.0f * sinf(a * 0.5f);
}

int main() {
  // 1) QMC5883L: определение, режим непрерывный, поле в мкТл
  reset(); field(20, 10, 40); AlashCompass c;
  CHECK(c.begin() && c.chip() == AlashCompass::CHIP_QMC5883L && c.address() == 0x0D);
  CHECK(g_cmp.ctrl1 == 0x1D);                                       // OSR 512, ±8 Гс, 200 Гц, непрерывно
  CHECK(near(c.x(), 20.0f, 0.05f) && near(c.y(), 10.0f, 0.05f) && near(c.z(), 40.0f, 0.05f));
  CHECK(near(c.fieldStrength(), sqrtf(20 * 20 + 10 * 10 + 40 * 40), 0.2f));

  // 2) HMC5883L: другой адрес и порядок X Z Y, те же значения для пользователя
  reset(false); field(20, 10, 40); AlashCompass h;
  CHECK(h.begin() && h.chip() == AlashCompass::CHIP_HMC5883L && h.address() == 0x1E);
  CHECK(near(h.x(), 20.0f, 0.1f) && near(h.y(), 10.0f, 0.1f) && near(h.z(), 40.0f, 0.1f));

  // 3) курс: восемь сторон света на обоих чипах
  struct { float x, y, deg; } k[] = {{20, 0, 0}, {20, 20, 45}, {0, 20, 90}, {-20, 20, 135}, {-20, 0, 180}, {-20, -20, 225}, {0, -20, 270}, {20, -20, 315}};
  for (bool qmc : {true, false}) {
    reset(qmc); AlashCompass a; a.begin(); a.setSmoothing(1);
    for (auto &t : k) { field(t.x, t.y); CHECK(angDiff(a.heading(), t.deg) < 0.5f); }
  }

  // 4) стороны света словами
  reset(); AlashCompass d; d.begin(); d.setSmoothing(1);
  field(20, 0); CHECK(strcmp((const char *)d.direction(), "Север") == 0 && strcmp((const char *)d.directionShort(), "С") == 0);
  field(0, 20); CHECK(strcmp((const char *)d.direction(), "Восток") == 0);
  field(-20, -20); CHECK(strcmp((const char *)d.direction(), "Юго-запад") == 0);
  d.setLanguage(AlashCompass::LANG_EN); field(20, 20);
  CHECK(strcmp((const char *)d.direction(), "North-east") == 0 && strcmp((const char *)d.directionShort(), "NE") == 0);
  d.setLanguage(AlashCompass::LANG_KK); field(-20, 0);
  CHECK(strcmp((const char *)d.direction(), "Оңтүстік") == 0);
  field(20, -2); CHECK(strcmp((const char *)d.direction(), "Солтүстік") == 0);       // 354 градуса это север, не западный сектор

  // 5) склонение, смещение, инверсия, переход через 0/360
  reset(); AlashCompass o; o.begin(); o.setSmoothing(1); field(20, 0);
  o.setDeclination(5.5f); CHECK(near(o.heading(), 5.5f, 0.3f));
  o.setDeclination(-8.0f); CHECK(near(o.heading(), 352.0f, 0.3f));
  o.setDeclination(0); o.setHeadingOffset(90); CHECK(near(o.heading(), 90.0f, 0.3f));
  o.setHeadingOffset(0); field(0, 20); o.invertHeading(); CHECK(near(o.heading(), 270.0f, 0.3f));

  // 6) усреднение: резкий скачок поля проходит плавно, потом доходит
  reset(); AlashCompass s; s.begin(); s.setSmoothing(4); field(20, 0);
  s.heading(); field(0, 20);
  float first = s.heading(); CHECK(first > 0 && first < 60);
  for (int i = 0; i < 40; i++) s.heading();
  CHECK(near(s.heading(), 90.0f, 1.0f));

  // 7) калибровка по вращению: после неё курс правильный на всех углах несмотря на железо рядом
  reset(); AlashCompass cal; cal.begin(); cal.setSmoothing(1);
  spinAngle = 0; g_onDelay = spin;
  CHECK(cal.calibrate(6000));
  g_onDelay = nullptr;
  float worstBefore = 0, worstAfter = 0;
  for (int deg = 0; deg < 360; deg += 15) {
    float a = deg * 3.14159265f / 180.0f;
    g_cmp.fx = 12.0f + 30.0f * 1.3f * cosf(a); g_cmp.fy = -7.0f + 30.0f * 0.8f * sinf(a);
    float e = angDiff(cal.heading(), (float)deg);
    if (e > worstAfter) worstAfter = e;
  }
  printf("  после калибровки худшая ошибка курса: %.2f градуса\n", worstAfter);
  CHECK(worstAfter < 3.0f);
  AlashCompass raw; raw.begin(); raw.setSmoothing(1);
  for (int deg = 0; deg < 360; deg += 15) {
    float a = deg * 3.14159265f / 180.0f;
    g_cmp.fx = 12.0f + 30.0f * 1.3f * cosf(a); g_cmp.fy = -7.0f + 30.0f * 0.8f * sinf(a);
    float e = angDiff(raw.heading(), (float)deg); if (e > worstBefore) worstBefore = e;
  }
  printf("  без калибровки худшая ошибка: %.2f градуса\n", worstBefore);
  CHECK(worstBefore > 10.0f && worstAfter < worstBefore / 3);

  // 8) калибровка без вращения не принимается
  reset(); field(20, 5); AlashCompass nr; nr.begin();
  CHECK(!nr.calibrate(1000) && nr.error() == AlashCompass::ERR_NOT_ROTATED);
  char buf[3]; (void)buf;
  nr.setCalibration(1, 2, 3, 1.1f, 0.9f, 1.0f); printf("--- calibration line ---\n"); nr.printCalibration(*(new Print));

  // 9) переполнение (рядом магнит) и ошибки
  reset(); field(20, 0); AlashCompass v; v.begin(); v.setSmoothing(1); g_cmp.overflow = true;
  CHECK(isnan(v.heading()) && v.error() == AlashCompass::ERR_OVERFLOW);
  g_cmp.overflow = false; CHECK(near(v.heading(), 0.0f, 0.5f) && v.error() == AlashCompass::ERR_OK);
  reset(false); g_cmp.overflow = true; AlashCompass v2; v2.begin(); CHECK(isnan(v2.heading()) && v2.error() == AlashCompass::ERR_OVERFLOW);
  reset(); g_cmp.present = false; AlashCompass n; CHECK(!n.begin() && n.error() == AlashCompass::ERR_NO_DEVICE && isnan(n.heading()));
  reset(); g_cmp.foreign = true; AlashCompass w; CHECK(!w.begin() && w.error() == AlashCompass::ERR_WRONG_CHIP);
  reset(); AlashCompass p; p.begin(); p.setSmoothing(1); p.heading(); g_cmp.present = false;
  CHECK(isnan(p.x()) == false || p.error() == AlashCompass::ERR_I2C);

  // 10) диагностика на трёх языках
  reset(); field(40, 0, 30); AlashCompass g; g.begin(); g.setLanguage(AlashCompass::LANG_KK); const char *kk = (const char *)g.errorText();
  g.setLanguage(AlashCompass::LANG_EN); g.diagnose(*(new Print));
  field(300, 0, 0); g.diagnose(*(new Print));                              // слишком сильное поле: подсказка
  CHECK(kk != nullptr);

  if (fails) { printf("FAILED: %d\n", fails); return 1; }
  printf("ALL TESTS PASSED\n");
  return 0;
}
