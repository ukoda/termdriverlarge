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


static uint32_t get_lamp_colour(uint8_t lamp)
{
//  uint32_t bightness = screen.lamps[lamp] >> 4;
//  return 0x0a000000 | (bightness << 8) | (bightness << 12) | (bightness << 16); // Black to white on green 

  uint32_t bightness = screen.lamps[lamp] >> 5; // White on dark green to light green
  return (bightness << 24) + 0x040fff00;
}


static void line_text(uint8_t *dst, int y)
{
  // line_text_4x6(dst, y);
  // line_text_6x9_sub(dst, y);
  line_text_serial(FontUbuntuMono_8, dst, y, screen.s, true);
}


static void line_status(uint8_t *dst, int y)
{
    uint32_t status[COLS * ROWS];   // TODO: Make smaller once we change to a bigger font
    const char txdstr[] = "TXD";
    const char rxdstr[] = "RXD";
    const char rtsstr[] = "RTS";
    const char dtrstr[] = "DTR";
    uint32_t lampb;

    uint8_t cp, x;

    memset(status, 0x00, sizeof(status));
    cp = 0;

    status[cp++] = 0x888fff20;    // Left border

    // TXD 'lamp'

    lampb = get_lamp_colour(LAMP_TX);
    for (x = 0; x < 3; x++) {
      status[cp++] = lampb | txdstr[x];
    }
    status[cp++] = 0x888fff20;  // Gap

    // RXD 'lamp'

    lampb = get_lamp_colour(LAMP_RX);
    for (x = 0; x < 3; x++) {
      status[cp++] = lampb | rxdstr[x];
    }
    status[cp++] = 0x888fff20;  // Gap

    // RTS 'lamp'

    lampb = get_lamp_colour(LAMP_RTS);
    for (x = 0; x < 3; x++) {
      status[cp++] = lampb | rtsstr[x];
    }
    status[cp++] = 0x888fff20;  // Gap

    // DTR 'lamp'

    lampb = get_lamp_colour(LAMP_DTR);
    for (x = 0; x < 3; x++) {
      status[cp++] = lampb | dtrstr[x];
    }
    status[cp++] = 0x888fff20;  // Gap

    // Serial port settings

    for (x = 0; screen.mode[x]; x++) {
      status[cp++] = 0x008fff00 | screen.mode[x];
    }

    // Fill to end of line

    for (x = 0; x < 11; x++)
      status[cp++] = 0x888fff20;

    // Do the actual render

    line_text_serial(FontFreeMono_12, dst, y, status, false);
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
