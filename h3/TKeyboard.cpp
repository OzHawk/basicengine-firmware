#include <TKeyboard.h>
#include "TPS2.h"
#include <usb.h>

TPS2 pb;

static const int ps2_to_usb[] = {
0,	// PS2KEY_NONE              0  // 継続またはバッファに変換対象なし
226,	// PS2KEY_L_Alt		    1	// [左Alt]
225,	// PS2KEY_L_Shift		  2	// [左Shift]
224,	// PS2KEY_L_Ctrl		  3	// [左Ctrl]
229,	// PS2KEY_R_Shift		  4	// [右Shift]
230,	// PS2KEY_R_Alt		    5	// [右Alt]
228,	// PS2KEY_R_Ctrl		  6	// [右Ctrl]
227,	// PS2KEY_L_GUI		    7	// [左Windowsキー]
231,	// PS2KEY_R_GUI		    8	// [右Windowsキー]
83,	// PS2KEY_NumLock		  9	// [NumLock]
71,	// PS2KEY_ScrollLock	10	// [ScrollLock]
57,	// PS2KEY_CapsLock	  11	// [CapsLock]
70,	// PS2KEY_PrintScreen	12	// [PrintScreen]
0,	// PS2KEY_HanZen		  13	// [半角/全角 漢字]
73,	// PS2KEY_Insert		  14	// [Insert]
74,	// PS2KEY_Home		    15	// [Home]
72,	// PS2KEY_Pause		    16	// [Pause]
0,	// PS2KEY_Romaji		  17	// [カタカナ ひらがな ローマ字]
0,	// PS2KEY_APP			    18	// [メニューキー]
0,	// PS2KEY_Henkan		  19	// [変換]
0,	// PS2KEY_Muhenkan	  20	// [無変換]
75,	// PS2KEY_PageUp		  21	// [PageUp]
78,	// PS2KEY_PageDown	  22	// [PageDown]
77,	// PS2KEY_End			    23	// [End]
80,	// PS2KEY_L_Arrow		  24	// [←]
82,	// PS2KEY_Up_Arrow	  25	// [↑]
79,	// PS2KEY_R_Arrow		  26	// [→]
81,	// PS2KEY_Down_Arrow	27	// [↓]
0,
0,
41,	// PS2KEY_ESC			    30	// [ESC]
43,	// PS2KEY_Tab 		    31	// [Tab]
40,	// PS2KEY_Enter       32  // [Enter]
42,	// PS2KEY_Backspace	  33	// [BackSpace]
76,	// PS2KEY_Delete		  34	// [Delete]
44,	// PS2KEY_Space		    35	// [空白]
52,	// PS2KEY_Colon		    36	// [: *]
51,	// PS2KEY_Semicolon	  37	// [; +]
54,	// PS2KEY_Kamma		    38	// [, <]
45,	// PS2KEY_minus		    39	// [- =]
55,	// PS2KEY_Dot			    40	// [. >]
56,	// PS2KEY_Question	  41	// [/ ?]
47,	// PS2KEY_AT			    42	// [@ `]
48,	// PS2KEY_L_brackets	43	// [[ {]
46,	// PS2KEY_Pipe		    44	// [\ |]
49,	// PS2KEY_R_brackets	45	// [] }]
0,	// PS2KEY_Hat			    46	// [^ ~]
0,	// PS2KEY_Ro			    47	// [\ _ ろ]
39,	// PS2KEY_0			48	// [0 )]
30,	// PS2KEY_1			49	// [1 !]
31,	// PS2KEY_2			50	// [2 @]
32,	// PS2KEY_3			51	// [3 #]
33,	// PS2KEY_4			52	// [4 $]
34,	// PS2KEY_5			53	// [5 %]
35,	// PS2KEY_6			54	// [6 ^]
36,	// PS2KEY_7			55	// [7 &]
37,	// PS2KEY_8			56	// [8 *]
38,	// PS2KEY_9			57	// [9 (]
#if 0
SDLK_BACKSLASH,	// PS2KEY_Pipe2 58  // [\ |] (USキーボード用)
0,
0,
0,
0,
0,
0,
SDLK_a,	// PS2KEY_A			65	// [a A]
SDLK_b,	// PS2KEY_B			66	// [b B]
SDLK_c,	// PS2KEY_C			67	// [c C]
SDLK_d,	// PS2KEY_D			68	// [d D]
SDLK_e,	// PS2KEY_E			69	// [e E]
SDLK_f,	// PS2KEY_F			70	// [f F]
SDLK_g,	// PS2KEY_G			71	// [g G]
SDLK_h,	// PS2KEY_H			72	// [h H]
SDLK_i,	// PS2KEY_I			73	// [i I]
SDLK_j,	// PS2KEY_J			74	// [j J]
SDLK_k,	// PS2KEY_K			75	// [k K]
SDLK_l,	// PS2KEY_L			76	// [l L]
SDLK_m,	// PS2KEY_M			77	// [m M]
SDLK_n,	// PS2KEY_N			78	// [n N]
SDLK_o,	// PS2KEY_O			79	// [o O]
SDLK_p,	// PS2KEY_P			80	// [p P]
SDLK_q,	// PS2KEY_Q			81	// [q Q]
SDLK_r,	// PS2KEY_R			82	// [r R]
SDLK_s,	// PS2KEY_S			83	// [s S]
SDLK_t,	// PS2KEY_T			84	// [t T]
SDLK_u,	// PS2KEY_U			85	// [u U]
SDLK_v,	// PS2KEY_V			86	// [v V]
SDLK_w,	// PS2KEY_W			87	// [w W]
SDLK_x,	// PS2KEY_X			88	// [x X]
SDLK_y,	// PS2KEY_Y			89	// [y Y]
SDLK_z,	// PS2KEY_Z			90	// [z Z]
0,
0,
0,
SDLK_EQUALS,	// PS2KEY_PAD_Equal	94	// [=]
SDLK_KP_ENTER,	// PS2KEY_PAD_Enter	95	// [Enter]
SDLK_KP0,	// PS2KEY_PAD_0		96  	// [0/Insert]
SDLK_KP1,	// PS2KEY_PAD_1		97  	// [1/End]
SDLK_KP2,	// PS2KEY_PAD_2		98  	// [2/DownArrow]
SDLK_KP3,	// PS2KEY_PAD_3		99  	// [3/PageDown]
SDLK_KP4,	// PS2KEY_PAD_4		100 	// [4/LeftArrow]
SDLK_KP5,	// PS2KEY_PAD_5		101 	// [5]
SDLK_KP6,	// PS2KEY_PAD_6		102 	// [6/RightArrow]
SDLK_KP7,	// PS2KEY_PAD_7		103 	// [7/Home]
SDLK_KP8,	// PS2KEY_PAD_8		104 	// [8/UPArrow]
SDLK_KP9,	// PS2KEY_PAD_9		105	  // [9/PageUp]
SDLK_KP_MULTIPLY,	// PS2KEY_PAD_Multi	106	// [*]
SDLK_KP_PLUS,	// PS2KEY_PAD_Plus	107	// [+]
0,	// PS2KEY_PAD_Kamma	108	// [,]
SDLK_KP_MINUS,	// PS2KEY_PAD_Minus	109	// [-]
SDLK_KP_PERIOD,	// PS2KEY_PAD_DOT		110	// [./Delete]
SDLK_KP_DIVIDE,	// PS2KEY_PAD_Slash	111	// [/]
SDLK_F1,	// PS2KEY_F1 		112	// [F1]
SDLK_F2,	// PS2KEY_F2 		113	// [F2]
SDLK_F3,	// PS2KEY_F3 		114	// [F3]
SDLK_F4,	// PS2KEY_F4 		115	// [F4]
SDLK_F5,	// PS2KEY_F5 		116	// [F5]
SDLK_F6,	// PS2KEY_F6 		117	// [F6]
SDLK_F7,	// PS2KEY_F7 		118	// [F7]
SDLK_F8,	// PS2KEY_F8 		119	// [F8]
SDLK_F9,	// PS2KEY_F9 		120	// [F9]
SDLK_F10,	// PS2KEY_F10 	121	// [F10]
SDLK_F11,	// PS2KEY_F11 	122	// [F11]
SDLK_F12,	// PS2KEY_F12 	123	// [F12]
SDLK_F13,	// PS2KEY_F13 	124	// [F13]
SDLK_F14,	// PS2KEY_F14 	125	// [F14]
SDLK_F15,	// PS2KEY_F15 	126	// [F15]
0,	// PS2KEY_F16 	127	// [F16]
0,	// PS2KEY_F17 	128	// [F17]
0,	// PS2KEY_F18 	129	// [F18]
0,	// PS2KEY_F19 	130	// [F19]
0,	// PS2KEY_F20 	131	// [F20]
0,	// PS2KEY_F21 	132	// [F21]
0,	// PS2KEY_F22 	133	// [F22]
0,	// PS2KEY_F23 	134	// [F23]
0,	// PS2KEY_PrevTrack		  135	// 前のトラック
0,	// PS2KEY_WWW_Favorites	136	// ブラウザお気に入り
0,	// PS2KEY_WWW_Refresh		137	// ブラウザ更新表示
0,	// PS2KEY_VolumeDown		138	// 音量を下げる
0,	// PS2KEY_Mute			    139	// ミュート
0,	// PS2KEY_WWW_Stop		  140	// ブラウザ停止
0,	// PS2KEY_Calc			    141	// 電卓
0,	// PS2KEY_WWW_Forward		142	// ブラウザ進む
0,	// PS2KEY_VolumeUp		  143	// 音量を上げる
0,	// PS2KEY_PLAY			    144	// 再生
SDLK_POWER,	// PS2KEY_POWER			    145	// 電源ON
0,	// PS2KEY_WWW_Back		  146	// ブラウザ戻る
0,	// PS2KEY_WWW_Home		  147	// ブラウザホーム
0,	// PS2KEY_Sleep			    148	// スリープ
0,	// PS2KEY_Mycomputer		149	// マイコンピュータ
0,	// PS2KEY_Mail			    150	// メーラー起動
0,	// PS2KEY_NextTrack		  151	// 次のトラック
0,	// PS2KEY_MEdiaSelect		152	// メディア選択
0,	// PS2KEY_Wake			    153	// ウェイクアップ
0,	// PS2KEY_Stop			    154	// 停止
0,	// PS2KEY_WWW_Search		155	// ウェブ検索
#endif
};

static const uint8_t usb2ascii[] = {
  0x00,0x00,0x00,0x00,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6A,0x6B,0x6C,
  0x6D,0x6E,0x6F,0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7A,0x31,0x32,
  0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x30,0x0D,0x1B,0x08,0x09,0x20,0x2D,0x3D,0x5B,
  0x5D,0x00,0x5C,0x3B,0x27,0x60,0x2C,0x2E,0x2F,0x00,0x81,0x82,0x83,0x84,0x85,0x86,
  0x87,0x88,0x89,0x8A,0x8B,0x8C,0x00,0x00,0x00,0x1A,0x0F,0x0B,0x7F,0x0E,0x0C,0x14,
  0x13,0x12,0x11,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4A,0x4B,0x4C,
  0x4D,0x4E,0x4F,0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5A,0x21,0x40,
  0x23,0x24,0x25,0x5E,0x26,0x2A,0x28,0x29,0x00,0x00,0x00,0x00,0x00,0x5F,0x2B,0x7B,
  0x7D,0x7C,0x00,0x3A,0x22,0x7E,0x3C,0x3E,0x3F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};

#define KEYBUF_SIZE 8
keyEvent keybuf[KEYBUF_SIZE];
int keybuf_r = 0;
int keybuf_w = 0;

static uint8_t key_state[256];
void hook_usb_keyboard_report(hid_keyboard_report_t *rep)
{
  uint8_t old_state[256];
  printf("kbdrep mod %02X keys ", rep->modifier);
  for (int i=0; i < 6; ++i) {
    printf("%02X ", rep->keycode[i]);
  }
  putchar('\n');

  memcpy(old_state, key_state, 256);
  memset(key_state, 0, 256);

  for (int i = 0; i < 6; ++i) {
    uint8_t kc = rep->keycode[i];
    key_state[kc] = 1;
    if (old_state[kc] != key_state[kc] && (keybuf_w + 1) % KEYBUF_SIZE != keybuf_r) {
      keyEvent *kev = &keybuf[keybuf_w];
      memset(kev, 0, sizeof(*kev));

      uint8_t kc_off = kc + ((rep->modifier & (2 | 32)) ? 0x80 : 0);
      if (kc_off < sizeof(usb2ascii))
        kev->code = usb2ascii[kc_off];
      else
        kev->code = 0;

      if (!kev->code) {
        kev->KEY = 1;
        for (size_t i = 0; i < sizeof(ps2_to_usb)/sizeof(*ps2_to_usb); ++i) {
          if (ps2_to_usb[i] == kc) {
            kev->code = i;
            break;
          }
        }
      }

      if (old_state[kc])
        kev->BREAK = 1;
      if (rep->modifier & (1 | 16))
        kev->CTRL = 1;
      if (rep->modifier & (2 | 32))
        kev->SHIFT = 1;
      if (rep->modifier & 4)
        kev->ALT = 1;
      if (rep->modifier & 64)
        kev->ALTGR = 1;

      keybuf_w = (keybuf_w + 1) % KEYBUF_SIZE;
    }
  }

  for (int i = 0; i < 8; ++i) {
    if (rep->modifier & (1 << i))
      key_state[224 + i] = 1;
  }

  key_state[0] = 0;
}

bool TKeyboard::state(uint8_t keycode) {
  if (keycode < sizeof(ps2_to_usb))
    return key_state[ps2_to_usb[keycode]];
  else
    return false;
}

uint8_t TPS2::available() {
  if (keybuf_w != keybuf_r)
    return 1;
  else
    return 0;
}

keyEvent TKeyboard::read() {
  if (keybuf_w != keybuf_r) {
    keyEvent *k = &keybuf[keybuf_r];
    keybuf_r = (keybuf_r + 1) % KEYBUF_SIZE;
    return *k;
  } else {
    keyEvent k;
    memset(&k, 0, sizeof(k));
    return k;
  }
}

uint8_t TKeyboard::begin(uint8_t clk, uint8_t dat, uint8_t flgLED, uint8_t layout) {
  return 0;
}

void TKeyboard::end() {
}
