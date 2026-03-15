#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "tusb.h"

#include "hardware/spi.h"
#include "hardware/dma.h"

#define MANUFACTURING 0

#include "../gen/guide240.i"

#include "drawscreen.h"
#include "text.h"
#include "state.h"
#include "panel.h"

#define spi spi1

#define USE_DMA 1

// #define PIN_MISO 13
#define PIN_SCK  14
#define PIN_MOSI 15
#define PIN_RES  11    // panel reset
#define PIN_DC   10    // panel Command/Data

// Map common command names to ST7796 opcodes.
#define ST7796_NOP      0x00
#define ST7796_SWRESET  0x01
#define ST7796_RDDID    0x04
#define ST7796_RDDST    0x09

#define ST7796_SLPIN    0x10
#define ST7796_SLPOUT   0x11
#define ST7796_PTLON    0x12
#define ST7796_NORON    0x13

#define ST7796_INVOFF   0x20
#define ST7796_INVON    0x21
#define ST7796_DISPOFF  0x28
#define ST7796_DISPON   0x29

#define ST7796_CASET    0x2A
#define ST7796_RASET    0x2B
#define ST7796_RAMWR    0x2C
#define ST7796_RAMRD    0x2E

#define ST7796_PTLAR    0x30
#define ST7796_SCRLAR   0x33
#define ST7796_MADCTL   0x36
#define ST7796_VSCSAD   0x37
#define ST7796_COLMOD   0x3A

#define ST7796_GSCAN    0x45
#define ST7796_WRDISBV  0x51

#define ST7796_FRMCTR1  0xB1
#define ST7796_FRMCTR2  0xB2
#define ST7796_FRMCTR3  0xB3
#define ST7796_INVCTR   0xB4
#define ST7796_DFC      0xB6

#define ST7796_PWR1     0xC0
#define ST7796_PWR2     0xC1
#define ST7796_PWR3     0xC2

#define ST7796_VCMPCTL  0xC5

#define ST7796_RDID1    0xDA
#define ST7796_RDID2    0xDB
#define ST7796_RDID3    0xDC

#define ST7796_GMCTRP1  0xE0
#define ST7796_GMCTRN1  0xE1

#define ST7796_DOCA     0xE8
#define ST7796_CSCON    0xF0



static void command(uint8_t x)
{
  gpio_put(PIN_DC, 0);
  spi_write_blocking(spi, &x, 1);
}

static void data1(uint8_t x)
{
  gpio_put(PIN_DC, 1);
  spi_write_blocking(spi, &x, 1);
}

static void datan(size_t n, const uint8_t *p)
{
  gpio_put(PIN_DC, 1);
  spi_write_blocking(spi, p, n);
}

static void set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
  command(ST7796_CASET);
  {
    uint8_t d[4] = {
      x0 >> 8, x0, x1 >> 8, x1
    };
    datan(sizeof(d), d);
  }
  command(ST7796_RASET);
  {
    uint8_t d[4] = {
      y0 >> 8, y0, y1 >> 8, y1
    };
    datan(sizeof(d), d);
  }
  command(ST7796_RAMWR);
}

static void cls()
{
  set_window(0, 0, LCD_W - 1, LCD_H - 1);
  for (int i = 0; i < (LCD_W * LCD_H * 2); i++)
    data1(0);
}

static void drawch(int x, int y, uint8_t ch)
{
  extern const uint8_t mainfont[], mainfont_decode[];
  uint32_t w = mainfont[0], h = mainfont[1];
  uint32_t bpl = (w * 3) / 2;
  uint32_t bpc = bpl * h;
  size_t ix = mainfont_decode[ch];

  set_window(x, y, x + w - 1, y + h - 1);
  const uint8_t *src = mainfont + 2 + (bpc * ix);
  for (uint32_t i = 0; i < (h * bpl); i++)
    data1(*src++);
}

static void show_code()
{
  extern char *td2_boardname();
  const int w = 16, n = 6;
  char *nm = td2_boardname();
  for (int i = 0; i < n; i++) {
    int x = (LCD_W - w * n) / 2 + (w * i);
    drawch(x, 5, nm[i]);
  }
}

static uint8_t linebuf[2][LCD_M * 2];


static void show_splash()
{
  uint16_t y = 180;
  set_window(0, y, GUIDE_W - 1, y + GUIDE_H - 1);

  uint32_t status[COLS * ROWS];   // TODO: Make smaller once we change to a bigger font
  const char txdstr[] = "TXD";
  const char rxdstr[] = "RXD";
  const char rtsstr[] = "RTS";
  const char dtrstr[] = "DTR";
  uint8_t dst[LCD_W];

  uint8_t cp;
  uint16_t x;

  for (y = 0; y < 20; y++) {
    memset(status, 0x00, sizeof(status));
    cp = 0;

  //  status[cp++] = 0x888fff20;    // Left border

    // TXD guide

    for (x = 0; x < 3; x++) {
      status[cp++] = 0x888fff00 | txdstr[x];
    }

    // RXD guide

    for (x = 0; x < 3; x++) {
      status[cp++] = 0x888fff00 | rxdstr[x];
    }
    line_text_serial(FontFreeMono_12, linebuf[0], y, status, false);
    linebuf[0][0] = 0xff;
    linebuf[0][1] = 0xff;
    linebuf[0][2] = 0xff;
    linebuf[0][3] = 0xff;
    datan(sizeof(linebuf[0]), linebuf[0]);

//    for (x = 0; x < LCD_W; x++)
//      data1(linebuf[0][x]);
  }

/*
  for (int i = 0; i < sizeof(guide_240); i++)
    data1(guide_240[i]);
*/

//  show_code();
}

volatile screen_t screen;
uint32_t screen_y;


#if USE_DMA
uint dma_tx;
dma_channel_config c;
#endif

static bool actual_dtr, actual_rts;

static void lamps_off(void)
{
  screen.lamps[LAMP_TX] = 0;
  screen.lamps[LAMP_RX] = 0;
  screen.lamps[LAMP_DTR] = 0;
  screen.lamps[LAMP_RTS] = 0;
}

static void lamps_age(void)
{
  inclamp(LAMP_TX, -15);
  inclamp(LAMP_RX, -15);
  inclamp(LAMP_DTR, actual_dtr ? 50 : -25);
  inclamp(LAMP_RTS, actual_rts ? 50 : -25);
}


static void render()
{
#if USE_DMA
  dma_channel_configure(dma_tx, &c,
                      &spi_get_hw(spi)->dr, // write address
                      linebuf[0], // read address
                      sizeof(linebuf[0]), // element count (each element is of size transfer_data_size)
                      false); // don't start yet
#endif

  set_window(0, 0, LCD_W - 1, LCD_H - 1);
  gpio_put(PIN_DC, 1);
  screen.y = screen_y;

  if (1) {
    for (int y = 0; y < LCD_H; y++) {
      int w = (y & 1);
      line1(linebuf[w], y);
#if !USE_DMA
      datan(sizeof(linebuf[0]), linebuf[w]);
#else
      dma_channel_wait_for_finish_blocking(dma_tx);
      dma_channel_transfer_from_buffer_now(dma_tx, linebuf[w], sizeof(linebuf[0]));
#endif
    }
  }
#if USE_DMA
  dma_channel_wait_for_finish_blocking(dma_tx);
#endif
}

static void spi_in()
{
  // gpio_set_dir(PIN_MISO, GPIO_IN);
  // gpio_set_dir(PIN_MISO, GPIO_IN);
  gpio_set_dir(PIN_SCK, GPIO_OUT);
  // gpio_set_function(PIN_MISO, GPIO_FUNC_SIO);
  gpio_set_function(PIN_MOSI, GPIO_FUNC_SIO);
  gpio_set_function(PIN_SCK, GPIO_FUNC_SIO);
}

static void spi_out()
{
  // gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
  gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
  gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
}

static uint32_t gscan()
{
  command(ST7796_GSCAN);
  gpio_put(PIN_DC, 1);

  spi_in();

#if 0
  uint8_t dd[256];
  memset(dd, 0x55, sizeof(dd));
  spi_read_blocking(spi, 0xff, dd, 10);
  for (int i = 0; i < 20; i++)
    printf("%02x\n", dd[i]);
#else
  static uint32_t mx;
  uint32_t v = 0;
  for (int i = 0; i < 16; i++) {
    gpio_put(PIN_SCK, 1);
    gpio_put(PIN_SCK, 0);
    // printf("%2d: %d %d\n", i, gpio_get(PIN_MISO), gpio_get(PIN_MOSI));
    sleep_us(1);
    asm volatile("nop \n nop \n nop");
    asm volatile("nop \n nop \n nop");
    asm volatile("nop \n nop \n nop");
    // printf("%d ", gpio_get(PIN_MOSI));
    v = (v << 1) | gpio_get(PIN_MOSI);
  }
  if (v > mx)
    mx = v;
  // printf("%10d: %6x %6x\n", time_us_32(), v, mx);
#endif
  spi_out();
  // printf("[%d] ", v);
  return v;
}

static void panel_refresh(void)
{
  while (screen.traffic == 0)
    ;
  while (1) {
    uint32_t t0 = time_us_32();

    render();
    if (!screen.freeze) {
      lamps_age();
    }

    uint32_t t1 = time_us_32();
    // printf("Took %u us\n", t1 - t0);

    screen.paused = 0;
    while (screen.pause)
      screen.paused = 1;
    continue;
    sleep_us(1);

    while (gscan() != 332)
      ;
    while (gscan() != 333)
      ;

  }
}

void panel_init()
{
#if USE_DMA
  // Grab some unused dma channels
  dma_tx = dma_claim_unused_channel(true);
  c = dma_channel_get_default_config(dma_tx);
  channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
  channel_config_set_dreq(&c, spi_get_dreq(spi, true));
#endif

  gpio_init(PIN_RES);
  gpio_set_dir(PIN_RES, GPIO_OUT);
  gpio_init(PIN_DC);
  gpio_set_dir(PIN_DC, GPIO_OUT);

  // Enable SPI 0 at 1 MHz and connect to GPIOs
  spi_init(spi, 62500000);

  spi_set_format(spi, 8, SPI_CPHA_1, SPI_CPOL_1, SPI_MSB_FIRST);
  // gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
  gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
  gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);

  gpio_put(PIN_RES, 1);
  gpio_put(PIN_RES, 0);
  sleep_ms(10);
  gpio_put(PIN_RES, 1);
  sleep_ms(50);

  // ST7796 initialization sequence
  sleep_ms(120);
  command(ST7796_SWRESET); // Software reset
  sleep_ms(120);

  command(ST7796_SLPOUT); // Sleep exit
  sleep_ms(120);

  command(ST7796_CSCON); // Command Set control
  data1(0xC3);   // Enable extension command 2 part I

  command(ST7796_CSCON); // Command Set control
  data1(0x96);   // Enable extension command 2 part II

  command(ST7796_MADCTL); // Memory Data Access Control
//  data1(0x48);   // MX, MY, MV RGB mode - Portrait (X-mirror, Top-left->bottom-right, RGB)
  data1(0xE8);   // MX, MY, MV RGB mode - Landscape (X-mirror, Y-mirror, Row/column exchange, Top-left->bottom-right, RGB)

  command(ST7796_COLMOD); // Interface Pixel Format
  data1(0x55);   // 16-bit interface color format

  command(ST7796_INVCTR); // Column inversion
  data1(0x01);   // 1-dot inversion

  command(ST7796_DFC); // Display Function Control
  data1(0x80);   // Bypass
  data1(0x02);   // Source output scan config
  data1(0x3B);   // LCD drive line

  command(ST7796_DOCA); // Display Output Ctrl Adjust
  {
    static const uint8_t d[] = { 0x40, 0x8A, 0x00, 0x00, 0x29, 0x19, 0xA5, 0x33 };
    datan(sizeof(d), d);
  }

  command(ST7796_PWR2); // Power control 2
  data1(0x06);

  command(ST7796_PWR3); // Power control 3
  data1(0xA7);

  command(ST7796_VCMPCTL); // VCOM Control
  data1(0x18);

  sleep_ms(120);

  // ST7796 Gamma Sequence (E0)
  command(ST7796_GMCTRP1);
  {
    static const uint8_t g1[] = { 0xF0,0x09,0x0B,0x06,0x04,0x15,0x2F,0x54,0x42,0x3C,0x17,0x14,0x18,0x1B };
    datan(sizeof(g1), g1);
  }

  // ST7796 Gamma Sequence (E1)
  command(ST7796_GMCTRN1);
  {
    static const uint8_t g2[] = { 0xE0,0x09,0x0B,0x06,0x04,0x03,0x2B,0x43,0x42,0x3B,0x16,0x14,0x17,0x1B };
    datan(sizeof(g2), g2);
  }

  sleep_ms(120);

  command(ST7796_CSCON); // Disable extension command 2 part I
  data1(0x3C);
  command(ST7796_CSCON); // Disable extension command 2 part II
  data1(0x69);
  sleep_ms(120);

  cls();
  show_splash();
  command(ST7796_DISPON); // Display on

  screen.freeze = 0;
  screen.cursor = 1;
  screen.l_dtr = 0xc;
  screen.l_rts = 0xa;

  lamps_off();
  strcpy((char*)screen.mode, "ready");
  text_init();
  show_cursor();
  screen.traffic = 1;

#if !MANUFACTURING
  multicore_launch_core1(panel_refresh);
#endif
}

void panel_line_coding(state_t const* line_coding)
{
  printf("CALLBACK tud_cdc_line_coding_cb\n");
  if (1 || tud_cdc_connected()) {
    // uint32_t bit_rate;
    // uint8_t  stop_bits; ///< 0: 1 stop bit - 1: 1.5 stop bits - 2: 2 stop bits
    // uint8_t  parity;    ///< 0: None - 1: Odd - 2: Even - 3: Mark - 4: Space
    // uint8_t  data_bits; ///< can be 5, 6, 7, 8 or 16

    uint32_t baud = line_coding->bit_rate;

    // Kludge.
    // Want to activate monitor on connection, but Linux sets up all terminals
    // on connection to 9600. So don't count a 9600 baud setting as "traffic"
    if (baud != 9600)
      screen.traffic = 1;

    if (baud < 10000)
      sprintf((char*)screen.mode, "%d", baud);
    else {
      uint32_t
        u = baud % 1000,
        t = (baud / 1000) % 1000,
        m = (baud / 1000000);
      if (baud < 1000000)
        sprintf((char*)screen.mode, "%u,%03u", t, u);
      else if ((u | t) == 0)
        sprintf((char*)screen.mode, "%uM", m);
      else
        sprintf((char*)screen.mode, "%u,%03u,%03u", m, t, u);
    }

    static const char* stops[] = {"1", "1%", "2"};

    char buf[5];
    sprintf(buf, " %d%c%s",
      line_coding->data_bits,
      "NOEMS"[line_coding->parity],
      stops[line_coding->stop_bits]);
    strcat((char*)screen.mode, buf);

    printf("%d %d%c%s\n",
      line_coding->bit_rate,
      line_coding->data_bits,
      "NOEMS"[line_coding->parity],
      stops[line_coding->stop_bits]);
 }
}



// draw text on the panel
// dir:'t' LAMP_TX, 'r' LAMP_RX
void panel_text(const uint8_t *tx_buf, size_t n, int dir)
{
  screen.traffic = 1;
  hide_cursor();
  for (size_t i = 0; i < n; i++) {
    text_ch(tx_buf[i]);
  }
  show_cursor();

  if (!screen.freeze) {
    int lamp;
    if (dir == 't')
      lamp = LAMP_TX;
    else
      lamp = LAMP_RX;
    inclamp(lamp, n * LAMP_ATTACK);
  }
}

////////////////////////////////////////////////////////

void panel_dtr_rts(bool dtr, bool rts)
{
  actual_dtr = dtr;
  actual_rts = rts;
}

void panel_freeze(void)
{
  screen.freeze = 1;
  lamps_off();
}

void panel_tick(void)
{
  lamps_age();
}

void panel_lamp_tx(void)
{
  inclamp(LAMP_TX, LAMP_ATTACK);
}

void panel_lamp_rx(void)
{
  inclamp(LAMP_RX, LAMP_ATTACK);
}

void panel_hide_cursor(void)
{
  screen.cursor = 0;
}
