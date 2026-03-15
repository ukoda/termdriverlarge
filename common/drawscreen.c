#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "drawscreen.h"
#include "panel.h"
#include "render_fonts.h"

extern screen_t screen;

#include "../gen/mainfont.i"
#include "../gen/lamps.i"

extern void line_text_4x6(uint8_t *dst, int y);
extern void line_text_6x9_sub(uint8_t *dst, int y);

#define PIXBYTES(n) ((n) * 3 / 2)

#define LAMP_BPL (LAMP_W * 3 / 2)



static void line_text(uint8_t *dst, int y)
{
  // line_text_4x6(dst, y);
  // line_text_6x9_sub(dst, y);
  line_text_serial(FontUbuntuMono_8, dst, y, screen.s, true);
}


static void line_status(uint8_t *dst, int y)
{
    uint32_t status[COLS * ROWS];   // TODO: Make smaller once we change to a bigger font
    const char statusstr[] = "TXD RXD RTS DTR";
    uint8_t cp, x;

    memset(status, 0x00, sizeof(status));
    cp = 0;

    status[cp++] = 0x888fff20;    // Left border

    // Activity 'lamps'

    for (x = 0; statusstr[x]; x++) {
      status[cp++] = 0x080fff00 | statusstr[x];
    }

    // Gap

    status[cp++] = 0x888fff20;

    // Serial port settings

    for (x = 0; screen.mode[x]; x++) {
      status[cp++] = 0x008fff00 | screen.mode[x];
    }

    // Gap

    status[cp++] = 0x888fff20;

    // Do the actual render

    line_text_serial(FontFreeMono_12, dst, y, status, false);
}


static void line_status_old(uint8_t *dst, int y)
{
  // Set background colour for status area

#define BGF 0x00
#define BGS 0x1f

  uint16_t xpos;

  // Set pixels on the line to the default background colour

  for (xpos = 0; xpos < (2 * LCD_W); xpos += 2) {
    dst[xpos]   = BGF;
    dst[xpos+1] = BGS;
  }

  // Write status test

  uint32_t w = mainfont[0], h = mainfont[1];  // 16, 25
  uint32_t bpl = (w * 3) / 2;                 // 24
  uint32_t bpc = bpl * h;                     // 600
  uint32_t y0 = 16 - h / 2;                   // 9.5 = 9
  uint32_t fy = (y - y0);
  const char *s = screen.mode;

  if (fy < h) {
    uint32_t x = 4;
    for (size_t i = 0; i < strlen(s); i++) {
      if (s[i] == ' ') {
        x += 8;
      } else if (s[i] == ',') {
        x += 3;
      } else {
        size_t ix = mainfont_decode[(uint8_t)s[i]];
        const uint8_t *src = mainfont + 2 + (bpc * ix) + (bpl * fy);
        x &= ~1;
        uint8_t *pd = dst + (x * 3 / 2);
        if ((y == 10 || y == 11) && s[i] == '9') {
          debug("%c *pd %02x, y %d, ix %d, scr[%d] %02x\n", s[i], pd[0], y, ix, 2 + (bpc * ix) + (bpl * fy), src[0]);
        }
        for (uint32_t px = 0; px < bpl; px++) {
          uint16_t a = (*src & 0xf) | ((*src & 0xf0) << 4);
          a = (MAINFONT_BG * 0x1010) - (a * MAINFONT_BG);
          if ((y == 10 || y == 11) && s[i] == '9') {
            debug("  px %03d, src %02x -> %04x , a %04x, pd %02x\n", px, src[0], (*src & 0xf) | ((*src & 0xf0) << 4), a, ((a & 0xf000) >> 8) | ((a & 0x00f0) >> 4));
          }
          if ((px+2)%3) {
            if (*src) {
              *pd++ = 0xff;
              *pd++ = 0xff;
            } else {
              // *pd++ = BGF;
              // *pd++ = BGS;
              *pd++ = 0xf8;
              *pd++ = 0x00;
            }
          }
          src++;
//          *pd++ = ((a & 0xf000) >> 8) | ((a & 0x00f0) >> 4);
        }
//        x += w - 1;
        x += bpl;
      }
    }
  }

  // Show activity 'lamps'
/*
  y0 = ((40 - 32) / 2);
  fy = y - y0;
  if (fy < 32) {
    uint32_t lo = (fy >> 4) << 1;
    for (int i = 0; i < 2; i++) {
      const uint8_t *src = lamps[lo + i][screen.lamps[lo + i] >> 4] + LAMP_BPL * (fy & 0xf);
      memcpy(dst + PIXBYTES(188) + LAMP_BPL * i, src, LAMP_BPL);
    }
  }
*/    
}

void line1(uint8_t *dst, int y)
{
  if (y < 3)
    line_blank(dst, STATUSBG);
  else if (y < 26)
    line_status(dst, y - 5);
  else if (y < 30)
    line_blank(dst, STATUSBG);
  else if (y < (LCD_H - 2)) // Tweak the '-' amount to stop echo at bottom of screen
    line_text(dst, y - 30); // Tweak the '-' amount to set start after line_status area
  else
    memset(dst, 0x00, 2 * LCD_W);
}

void inclamp(size_t i, int32_t n)
{
  int32_t v = screen.lamps[i] + n;
  if (v < 0)
    v = 0;
  else if (v > 255)
    v = 255;
  screen.lamps[i] = v;
}
