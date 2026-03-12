#ifndef drawscreen_h
#define drawscreen_h
#include <stdint.h>

// Smallest font is 8 x 16 UbuntuMono_8

// WARNING: Code in text.c is using 0x1f as bit mask often

#define LCD_W 240 // Should be 480 or 320
#define LCD_H 240 // Should be 320 or 480
#define LCD_M 240 // The larger of LCD_W and LCD_H, should be 480
#define COLS  30  // Should be landscape 480/8  = 60, portait 320/8  = 40
#define ROWS  12  // Should be landscape 320/16 = 20, portait 480/16 = 30
// With smallest font 8 x 16 UbuntuMono_8
// Landscape would be 60 characters by 20 lines
// Portait would be   40 characters by 30 lines

#define LAMP_TX   0
#define LAMP_RX   1
#define LAMP_RTS  2
#define LAMP_DTR  3

#define LAMP_ATTACK 160

typedef struct {
  uint32_t traffic;
  uint32_t cols, rows;
  uint32_t y;
  uint32_t s[COLS * ROWS];
  int32_t lamps[4];
  int32_t freeze;
  int32_t cursor;
  uint32_t l_dtr, l_rts;
  uint32_t pause, paused;
  // 1,234,567 8N1
  char mode[14];
} screen_t;

void line1(uint8_t *dst, int y);
void inclamp(size_t i, int32_t n);
void debug(const char *fmt, ...);
#endif
