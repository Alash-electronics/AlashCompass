// Минимальная имитация Arduino.h для запуска тестов на компьютере (g++).
#pragma once
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
using __FlashStringHelper = char;
#define F(x) ((const __FlashStringHelper *)(x))
#define HEX 16
inline uint32_t g_ms = 100000;
inline uint32_t millis() { return g_ms; }
inline void (*g_onDelay)() = nullptr;      // вызывается при каждом delay(): имитация вращения платы во время калибровки
inline void delay(uint32_t ms) { g_ms += ms; if (g_onDelay) g_onDelay(); }
#include <string>
class Print {
 public:
  std::string text;
  void put(const char *s) { printf("%s", s); text += s; }
  void print(const char *s) { put(s); }
  void print(char c) { char b[2] = {c, 0}; put(b); }
  void print(unsigned v, int base = 10) { char b[24]; snprintf(b, sizeof b, base == HEX ? "%X" : "%u", v); put(b); }
  void print(int v, int base = 10) { char b[24]; snprintf(b, sizeof b, base == HEX ? "%X" : "%d", v); put(b); }
  void print(float v, int d = 2) { char b[40]; snprintf(b, sizeof b, "%.*f", d, v); put(b); }
  void println(const char *s = "") { put(s); put("\n"); }
  void println(unsigned v, int base = 10) { print(v, base); put("\n"); }
  void println(int v, int base = 10) { print(v, base); put("\n"); }
  void println(float v, int d = 2) { print(v, d); put("\n"); }
};
