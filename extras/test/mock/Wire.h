// Имитация I2C и компасов QMC5883L (0x0D) и HMC5883L (0x1E): поле в мкТл, порядок байтов настоящих чипов.
#pragma once
#include "Arduino.h"
struct FakeCompass {
  bool present = true; bool qmc = true; uint8_t addr = 0x0D; bool foreign = false;
  float fx = 20, fy = 0, fz = 40;     // магнитное поле, мкТл
  bool overflow = false;
  uint8_t ctrl1 = 0;
};
inline FakeCompass g_cmp;
class TwoWire {
 public:
  void begin() {}
  void begin(int, int) {}
  void beginTransmission(uint8_t a) { _a = a; _n = 0; }
  size_t write(uint8_t v) { if (_n == 0) _reg = v; else if (_a == 0x0D && _reg == 0x09) g_cmp.ctrl1 = v; _n++; return 1; }
  uint8_t endTransmission(bool = true) { return (g_cmp.present && _a == g_cmp.addr) ? 0 : 2; }
  uint8_t requestFrom(uint8_t a, uint8_t len) {
    if (!(g_cmp.present && a == g_cmp.addr)) return 0;
    for (uint8_t i = 0; i < len; i++) _buf[i] = reg(_reg + i);
    _len = len; _pos = 0; return len;
  }
  int read() { return _pos < _len ? _buf[_pos++] : -1; }
 private:
  uint8_t _a = 0, _reg = 0, _n = 0, _buf[16], _len = 0, _pos = 0;
  static int16_t cnt(float uT, float lsbPerGauss) { float c = uT / 100.0f * lsbPerGauss; if (c > 32767) c = 32767; if (c < -32768) c = -32768; return (int16_t)lroundf(c); }
  uint8_t reg(uint8_t r) {
    if (g_cmp.foreign) return r == 0x0D ? 0x00 : 0x00;
    if (g_cmp.qmc && g_cmp.addr == 0x0D) {
      int16_t x = cnt(g_cmp.fx, 3000), y = cnt(g_cmp.fy, 3000), z = cnt(g_cmp.fz, 3000);
      switch (r) {
        case 0x00: return x & 0xFF; case 0x01: return (x >> 8) & 0xFF;
        case 0x02: return y & 0xFF; case 0x03: return (y >> 8) & 0xFF;
        case 0x04: return z & 0xFF; case 0x05: return (z >> 8) & 0xFF;
        case 0x06: return 0x01 | (g_cmp.overflow ? 0x02 : 0);
        case 0x0D: return 0xFF;
      }
      return 0;
    }
    int16_t x = cnt(g_cmp.fx, 1090), y = cnt(g_cmp.fy, 1090), z = cnt(g_cmp.fz, 1090);
    if (g_cmp.overflow) x = -4096;
    switch (r) {
      case 0x03: return (x >> 8) & 0xFF; case 0x04: return x & 0xFF;     // X
      case 0x05: return (z >> 8) & 0xFF; case 0x06: return z & 0xFF;     // Z
      case 0x07: return (y >> 8) & 0xFF; case 0x08: return y & 0xFF;     // Y
      case 0x09: return 0x01;
      case 0x0A: return 'H'; case 0x0B: return '4'; case 0x0C: return '3';
    }
    return 0;
  }
};
inline TwoWire Wire;
