#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "render_fonts.h"
#include "drawscreen.h"
#include "UbuntuMono_8.h"

extern screen_t screen;


static uint16_t fg565 = 0xffff;
static uint16_t bg565 = 0x0000;

static uint32_t debuglines = 0;



/*
  convert444to565 routine

  This routine takes a 12 bit 4:4:4 RGB pixel data and converts it to the 16 bit
  5:6:5 format.

    15   14   13   12   11   10    9    8    7    6    5    4    3    2    1    0
  |    |    |    |    | R3 | R2 | R1 | R0 | G3 | G2 | G1 | G0 | B3 | B2 | B1 | B0 |
                         |              |    |              |    |              |
     /------- 4 << ------/              |    |              |    |              |
     |              /----+--------------/    |              |    |              |
     |              |    |                   |              |    |              |
     |              |    |    /---- 3 << ----/              |    |              |
     |              |    |    |              /----+----+----/    |              |
     |              |    |    |              |    |    |         |              |
     |              |    |    |              |    |    |    /- 1-/              |
     |              |    |    |              |    |    |    |              /----+
     |              |    |    |              |    |    |    |              |    |
  | R4 | R3 | R2 | R1 | R0 | G5 | G4 | G3 | G2 | G1 | G0 | B4 | B3 | B2 | B1 | B0 |
*/
uint16_t convert444to565(uint16_t pixel)
{
  uint16_t px;

  // Shift the significant bits and extend blue LSB
  px = ((pixel & 0x0f00) << 4) |
       ((pixel & 0x00f0) << 3) |
       ((pixel & 0x000f) << 1) |
        (pixel & 0x0001);
  // Extend red LSB
  if (pixel & 0x0100)
    px |= 0x0800;
  // Extend green LSB
  if (pixel & 0x0010)
    px |= 0x0060;

  return px;
}



/*
  pixel_foreground routine

  This routine saves a pixel in the foreground colour.
*/
void pixel_foreground(uint8_t *dst)
{
  *dst++ = fg565 >> 8;
  *dst++ = fg565;
}



/*
  pixel_background routine

  This routine saves a pixel in the background colour.
*/
void pixel_background(uint8_t *dst)
{
  *dst++ = bg565 >> 8;
  *dst++ = bg565;
}



/*
  line_text_serial routine

  This routine loads dst buffer with a line of pixel data that will be DMA'd
  to the LCD.
*/
void line_text_serial(TFont fontnum, uint8_t *dst, int y)
{
  const GFXfont *font;
  GFXglyph *glyph;
	uint8_t  *bitmap;
	uint16_t bs, bo;
	uint8_t  w, h, xa, be;
	int8_t   xo, yo;
	uint8_t  xx, yy, bits, bit;
	int16_t  xo16, yo16;
	uint16_t ft;
	uint16_t fh;

  // Copy some hardcoded values to vars to make future dynamic font changes easier

  // Font choice

  switch (fontnum) {
    default:
      font = &UbuntuMono_8;
  }

  // Fore ground and back ground colours in 12 bit and 16 bit formats

  uint16_t fg444 =  0xfff;
  uint16_t bg444 =  0x000;

  // Work out some stuff we will need

  uint8_t *dp = dst;
  uint16_t yi = y / font->yAdvance;
  uint32_t ymod = y % font->yAdvance;
  uint32_t *psrc = screen.s + (screen.cols * ((yi + screen.y) % screen.rows));
  uint8_t  lt, lb, yp;
  bool     le;


  // For each character in the line of the screen buffer

  for (uint8_t col = 0; col < screen.cols; col++) {
    uint32_t el = psrc[col];          // Get the character element with it's 12 bit colours
    uint16_t fg = (el >> 8) & 0xfff;  // 12 bit fore ground colour
    uint16_t bg = (el >> 20) & 0xfff; // 12 bit back ground colour
    uint8_t ch = el & 0xff;           // The 8 bit ASCII character

    // Create the 5:6:5 values for the foreground and background based on the 4:4:4 value in the buffer if it has changed

    if (fg != fg444) {
      fg444 = fg;
      fg565 = convert444to565(fg444);
    }
    if (bg != bg444) {
      bg444 = bg;
      bg565 = convert444to565(bg444);
    }

    // Convert character to position in font's character set.  If not in character set use font's first character

  	if ((ch < font->first) || (ch >= font->last))
		  ch = 0;
    else
    	ch -= font->first;
  
    // Get the pixels for the line of the character

    glyph = &font->glyph[ch];
    bitmap = font->bitmap;
    w    = glyph->width;

/*
bitmapOffset;     // Pointer into GFXfont->bitmap
width, height;    // Bitmap dimensions in pixels
xAdvance;         // Distance to advance cursor (x axis)
xOffset, yOffset; // Dist from cursor pos to UL corner
*/
//                *                *  16
// Offset    w    h  adv     x     y            h+y H-h  lt lb 11+y
//{   249,   6,  10,   8,    1,   -9 }   // 'H'  1   6  (2, 4)  2
//{   469,   6,   7,   8,    1,   -6 }   // 'e'  1   9  (5, 4)  5
//{   485,   6,  10,   8,    1,   -6 }   // 'g'  4   6  (5, 1)  5

    // Work out it this line of the character is displayed
    lt   = font->yAbove + glyph->yOffset;
    lb   = font->yAdvance - glyph->height - lt;
    le   = (ymod < lt) || (ymod > (font->yAdvance - lb));

    if (le) {
/*      
      if ((((el & 0xff) == 'H') || ((el & 0xff) == 'e')) && ((col == 0) || (col == 1)) && (debuglines++ < 50))
        debug("c=%c, y=%d, yi=%d, ymod=%d, lt=%d, lb=%d, empty\n",(el & 0xff) , y, yi, ymod, lt, lb);
*/        
      for (xx = 0; xx < glyph->xAdvance; xx++)
//        *dp++ = 0x42;
//        *dp++ = 0x80;
        *dp++ = 0x00;
        *dp++ = 0x00;
    } else {
      // Work out pixel related stuff

      yp   = ymod - lt;
      bs   = (w * yp) / 8;
      bo   = glyph->bitmapOffset + bs;
      bit  = (w * yp) % 8;
      if (bit)
        bits = bitmap[bo++] << bit;
      else
        bits = 0x00;
      be   = 8 - w - glyph->xOffset;

//      if (((el & 0xff) == '0') && (col == 0) && (debuglines++ < 20))
//        debug("y=%d, yi=%d, ymod=%d, bs=%d, bo=%d, w=%d, bit=%d, bits=%02x\n", y, yi, ymod, bs, bo, w, bit, bits);
/*
      if ((((el & 0xff) == 'H') || ((el & 0xff) == 'e')) && ((col == 0) || (col == 1)) && (debuglines++ < 50))
        debug("c=%c, y=%d, yi=%d, ymod=%d, yp=%d, lt=%d, lb=%d, pixels\n", (el & 0xff), y, yi, ymod, yp, lt, lb);
*/
      // Send background pixels before glyph data

      for (xx = 0; xx < glyph->xOffset; xx++) {
  //      pixel_background(dp);
//        *dp++ = 0xf8;
//        *dp++ = 0x00;
        *dp++ = 0x00;
        *dp++ = 0x00;
      }

      // Send pixels from glyph data

      for (xx=0; xx<w; xx++) {
        if (!(bit++ & 7)) {
          bits = bitmap[bo++];
  //        if (((el & 0xff) == '0') && (col == 0) && (debuglines < 50))
  //          debug("  bits=%02x\n", bits);
        }
        if (bits & 0x80) {
            *dp++ = fg565 >> 8;
            *dp++ = fg565;
        } else {
            *dp++ = bg565 >> 8;
            *dp++ = bg565;
            // *dp++ = 0x00;
            // *dp++ = 0x00;
        }
        bits <<= 1;
      }

      // Send background pixels after glyph data

      for (xx = 0; xx < be; xx++) {
  //      pixel_background(dp);
//        *dp++ = 0x00;
//        *dp++ = 0x1f;
        *dp++ = 0x00;
        *dp++ = 0x00;
      }
    }
  }
}
