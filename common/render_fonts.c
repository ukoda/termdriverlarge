#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "render_fonts.h"
#include "drawscreen.h"
#include "UbuntuMono_8.h"
#include "FreeMono_12.h"

extern screen_t screen;


static uint16_t fg565 = 0xffff;
static uint16_t bg565 = 0x0000;
static uint16_t pixelpos;



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
uint8_t* pixel_foreground(uint8_t *dst)
{
  if (pixelpos >= LCD_W)
    return dst;
  *dst++ = fg565 >> 8;
  *dst++ = fg565;
  pixelpos++;
  return dst;
}



/*
  pixel_background routine

  This routine saves a pixel in the background colour.
*/
uint8_t* pixel_background(uint8_t *dst)
{
  if (pixelpos >= LCD_W)
    return dst;
  *dst++ = bg565 >> 8;
  *dst++ = bg565;
  pixelpos++;
  return dst;
}



void line_blank(uint8_t *dst, uint16_t colour)
{
  uint8_t *dp = dst;
  uint16_t pixel;

  bg565 = colour;

  pixelpos = 0;
  for (pixel = 0; pixel < LCD_W; pixel++)
    dp = pixel_background(dp);  
}

/*
  line_text_serial routine

  This routine loads dst buffer with a line of pixel data that will be DMA'd
  to the LCD.
*/
void line_text_serial(TFont fontnum, uint8_t *dst, int y, uint32_t *src, bool wrap)
{
  const GFXfont *font;
  GFXglyph *glyph;
	uint8_t  *bitmap;
	uint16_t bs, bo;
	uint16_t w, h, xa, be;
	int16_t  xo, yo;
	uint16_t xx, yy;
  uint8_t  bits, bit;
	int16_t  xo16, yo16;
	uint16_t ft;
	uint16_t fh;
  uint32_t *psrc;

  // Font choice

  switch (fontnum) {
    case FontFreeMono_12:
      font = &FreeMono_12;
      break;

    default:
      font = &UbuntuMono_8;
      break;
  }

  pixelpos = 0;

  // Set 12 bit foreground and background invalid to force update

  uint16_t fg444 =  0xbad0;
  uint16_t bg444 =  0xbad0;

  // Work out some stuff we will need

  uint8_t *dp = dst;
  uint16_t yi = y / font->yAdvance;
  uint32_t ymod = y % font->yAdvance;
//  uint32_t *psrc = screen.s + (screen.cols * ((yi + screen.y) % screen.rows));
  if (wrap)
    psrc = src + (screen.cols * ((yi + screen.y) % screen.rows));
  else
    psrc = src;
  uint16_t lt, lb, yp;
  bool     le;


  // For each character in the line of the screen buffer

  for (uint16_t col = 0; (col < screen.cols) && (pixelpos < LCD_W); col++) {
    uint32_t el = psrc[col];          // Get the character element with it's 12 bit colours
    uint16_t fg = (el >> 8) & 0xfff;  // 12 bit fore ground colour
    uint16_t bg = (el >> 20) & 0xfff; // 12 bit back ground colour
    uint8_t  ch = el & 0xff;          // The 8 bit ASCII character

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

    // Work out if this line of the character is displayed
    lt   = font->yAbove + glyph->yOffset;                  // Blank lines above glyph
    lb   = font->yAdvance - glyph->height - lt;            // Lowest line with pixels for glyph
    le   = (ymod < lt) || (ymod >= (font->yAdvance - lb)); // Line has no pixels as is above or below glyph

    if (le) {
      for (xx = 0; xx < glyph->xAdvance; xx++)
        dp = pixel_background(dp);
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
      be   = glyph->xAdvance - w - glyph->xOffset;

      // Send background pixels left of the glyph data

      for (xx = 0; xx < glyph->xOffset; xx++) {
        dp = pixel_background(dp);
      }

      // Send pixels from glyph data

      for (xx=0; xx<w; xx++) {
        if (!(bit++ & 7)) {
          bits = bitmap[bo++];
        }
        if (bits & 0x80) {
          dp = pixel_foreground(dp);
        } else {
          dp = pixel_background(dp);
        }
        bits <<= 1;
      }

      // Send background pixels right of glyph data

      for (xx = 0; xx < be; xx++) {
        dp = pixel_background(dp);
      }
    }
  }
}

//#define ENABLE_PARAMETER_GATHER 1
/*
  display_fontParameter routine

  This routine is not intended for normal code.  It is used when a font is first
  used to determine the yAbove and yBelow values for in the GFXfont struct
  which are not part of the original Adafuit struct so not generated by
  fontconvert tool.
*/
#ifdef ENABLE_PARAMETER_GATHER
void display_fontParameter(void)
{
  GFXfont *font = (GFXfont *)&FreeMono_12;
	if (font == NULL)
		font = (GFXfont *)&UbuntuMono_8;

	uint8_t c, abovec = 0, belowc = 0;
	int8_t above = 0, below = 0;
	GFXglyph *glyph;

	for (c = font->first; c <= font->last; c++) {
		glyph  = &font->glyph[c - font->first];
		if ((glyph->yOffset * -1) > above) {
			above = glyph->yOffset * -1;
			abovec = c;
		}
		if ((glyph->yOffset + glyph->height) > below) {
			below = glyph->yOffset + glyph->height;
			belowc = c;
		}
	}
	debug("display_fontParameter yAbove = %d for %02x '%c', yBelow = %d for %02x '%c'\n",
			above, abovec, abovec, below, belowc, belowc);
}
#endif
