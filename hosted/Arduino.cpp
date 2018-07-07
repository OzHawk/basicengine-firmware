#include "Arduino.h"
#include "SdFat.h"
#include "FS.h"
#include "SPI.h"
#include <vs23s010.h>

void digitalWrite(uint8_t pin, uint8_t val) {
  printf("DW %d <- %02X\n", pin, val);
}

void pinMode(uint8_t pin, uint8_t mode) {
}

uint32_t timer0_read() {
  return 0;
}
void timer0_write(uint32_t count) {
}

void timer0_isr_init() {
}
void timer0_attachInterrupt(timercallback userFunc) {
}
void timer0_detachInterrupt(void) {
}

fs::FS SPIFFS;
SPIClass SPI;

void loop();
void setup();

SDL_Surface *screen;
SDL_Color palette[2][256];

struct palette {
  uint8_t r, g, b;
};
#include <N-0C-B62-A63-Y33-N10.h>
#include <P-EE-A22-B22-Y44-N10.h>

#define SDL_X_SIZE 1400
#define SDL_Y_SIZE 1050
#define STRETCH_Y 4.4	// empircally determined
#define YUV_Y_SIZE (int(SDL_Y_SIZE/STRETCH_Y))

#define VIEWPORT_X 308
#define VIEWPORT_Y 22

int main(int argc, char **argv)
{
  SDL_Init(SDL_INIT_EVERYTHING);
  SDL_EnableUNICODE(1);
  SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
  screen = SDL_SetVideoMode(SDL_X_SIZE, SDL_Y_SIZE, 32, SDL_HWSURFACE);
  if (!screen) {
    fprintf(stderr, "SDL set mode failed: %s\n", SDL_GetError());
    exit(1);
  }
  atexit(SDL_Quit);
  for (int i = 0; i < 256; ++i) {
    palette[0][i].r = n_0c_b62_a63_y33_n10[i].r;
    palette[0][i].g = n_0c_b62_a63_y33_n10[i].g;
    palette[0][i].b = n_0c_b62_a63_y33_n10[i].b;
    palette[1][i].r = p_ee_a22_b22_y44_n10[i].r;
    palette[1][i].g = p_ee_a22_b22_y44_n10[i].g;
    palette[1][i].b = p_ee_a22_b22_y44_n10[i].b;
  }
  setup();
  for (;;)
    loop();
}

#include "border_pal.h"

void hosted_pump_events() {
  static int last_line = 0;
  static SDL_Color *pl = palette[0];
  SDL_Event event;
  SDL_PumpEvents();
  while (SDL_PeepEvents(&event, 1, SDL_GETEVENT, SDL_ALLEVENTS ^ (SDL_KEYUPMASK|SDL_KEYDOWNMASK)) == 1) {
    switch (event.type) {
      case SDL_QUIT:
        exit(0);
        break;
      default:
        //printf("SDL event %d\n", event.type);
        break;
    }
    SDL_PumpEvents();
  }

  int new_line = SpiRamReadRegister(CURLINE);

  if (new_line > last_line) {
    // draw picture lines
    for (int i = last_line; i < vs23.height(); ++i) {
#define m_current_mode vs23.m_current_mode	// needed by STARTLINE...
#define m_pal vs23_int.pal

      // pointer to SDL screen line
      uint8_t *scr_line = (uint8_t *)screen->pixels +
                          (int((i + STARTLINE - VIEWPORT_Y) *STRETCH_Y))*screen->pitch;
      // X offset of start of picture (pixels)
      int xoff = vs23_int.picstart * 8 - VIEWPORT_X;
      // draw SDL screen border
      for (int j = 0; j < STRETCH_Y; ++j) {
        memcpy(scr_line + j * screen->pitch, screen->pixels, xoff * screen->format->BytesPerPixel);
        memcpy(scr_line + j * screen->pitch + (xoff + vs23.width()) * screen->format->BytesPerPixel,
               (uint8_t *)screen->pixels + (xoff + vs23.width()) * screen->format->BytesPerPixel,
               (screen->w - vs23.width() - xoff) * screen->format->BytesPerPixel);
      }


      // one SDL pixel is one VS23 PLL clock cycle wide
      // picstart is defined in terms of color clocks (PLL/8)
      uint8_t *scr = scr_line + xoff * screen->format->BytesPerPixel;

      uint8_t *vdc = vs23_mem + vs23.piclineByteAddress(i);
      uint8_t *sscr = scr;
      for (int x = 0; x < vs23.width(); ++x) {
        // pixel width determined by VS23 program length
        for (int p = 0; p < vs23_int.plen; ++p) {
          *scr++ = pl[vdc[x]].b;
          *scr++ = pl[vdc[x]].g;
          *scr++ = pl[vdc[x]].r;
          *scr++ = 0;
        }
      }
      scr = sscr + screen->pitch;
      for (int j = 1; j < STRETCH_Y; ++j, scr += screen->pitch) {
        memcpy(scr, sscr,
               vs23.width() * screen->format->BytesPerPixel * vs23_int.plen);
      }
    }
  } else if (new_line < last_line) {
    // frame complete, blit it

    // update palette
    int pal = vs23_mem[PROTOLINE_BYTE_ADDRESS(0) + BURST*2];
    if (pal == 0x0c || pal == 0xdd)
      pl = palette[0];
    else
      pl = palette[1];

    SDL_Flip(screen);
    uint8_t *p = (uint8_t*)screen->pixels;
    for (int x = 0; x < screen->w; ++x) {
      int w = PROTOLINE_WORD_ADDRESS(0) + BLANKEND + x/8;
      int y = (vs23_mem[w*2+1] - 0x66) * 255 / 0x99;
      int uv = vs23_mem[w*2];

      // couldn't find a working transformation, using a palette
      // generated by VS23Palette06.exe
      y /= 4;	// palette uses 6-bit luminance
      int v = 15 - (uv >> 4);
      int u = 15 - (uv & 0xf);
      int idx = ((u&0xf) << 6) | ((v& 0xf) << 10) | y;
      int val = border_pal[idx];
      int r = val & 0xff;
      int g = (val >> 8) & 0xff;
      int b = (val >> 16) & 0xff;

      *p++ = b;
      *p++ = g;
      *p++ = r;
      *p++ = 0;
    }

    // Left/right border is drawn at the time the respective
    // picture line is; here, we only draw protolines.

    // draw SDL screen border top (before start of picture)
    for (int i = 1; i < (STARTLINE - VIEWPORT_Y)*STRETCH_Y; ++i) {
      memcpy(p, screen->pixels, screen->pitch);
      p += screen->pitch;
    }
    // draw SDL screen border bottom (after end of picture)
    for (int i = (STARTLINE - VIEWPORT_Y+vs23.height()) * STRETCH_Y; i < screen->h; ++i) {
      memcpy((uint8_t *)screen->pixels + i * screen->pitch, screen->pixels, screen->pitch);
      p += screen->pitch;
    }
  }
  last_line = new_line;
  vs23.setFrame(micros() / vs23_int.line_us / vs23_int.line_count);
}

#include "Wire.h"
TwoWire Wire;
