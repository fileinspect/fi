#ifndef FI_TERM_H
#define FI_TERM_H


typedef enum TermResponse {
  TermSuccess = 0,
  TermFailure = -1
} TermResponse;

typedef unsigned int TermColor;
typedef enum TermColor16 {
  TermBlack = 0,
  TermRed,
  TermGreen,
  TermYellow,
  TermBlue,
  TermMagenta,
  TermCyan,
  TermWhite,
  TermBrightBlack,
  TermBrightRed,
  TermBrightGreen,
  TermBrightYellow,
  TermBrightBlue,
  TermBrightMagenta,
  TermBrightCyan,
  TermBrightWhite
} TermColor16;

typedef unsigned int TermKey;
typedef enum TermKeyValue {
  // Runes
  TermRuneMask = 0xFFu,
  TermRuneClear = 0x7FFFFF00u,
  // Special Keys
  TermSpecialMask = 0xFFF00u,
  TermSpecialClear = 0x7FF000FFu,
  TermArrowUp = 0x00100u,
  TermArrowDown = 0x00200u,
  TermArrowLeft = 0x00300u,
  TermArrowRight = 0x00400u,
  TermHome = 0x00500u,
  TermEnd = 0x00600u,
  TermPageUp = 0x00700u,
  TermPageDown = 0x00800u,
  TermInsert = 0x00900u,
  TermDelete = 0x00A00u,
  TermBackspace = 0x00B00u,
  TermEscape = 0x00C00u,
  TermEnter = 0x00D00u,
  TermTab = 0x00E00u,
  TermBacktab = 0x00F00u,
  TermF1 = 0x01000u,
  TermF2 = 0x01100u,
  TermF3 = 0x01200u,
  TermF4 = 0x01300u,
  TermF5 = 0x01400u,
  TermF6 = 0x01500u,
  TermF7 = 0x01600u,
  TermF8 = 0x01700u,
  TermF9 = 0x01800u,
  TermF10 = 0x01900u,
  TermF11 = 0x01A00u,
  TermF12 = 0x01B00u,
  // Modifier Keys
  TermModifierMask = 0xF00000u,
  TermModifierClear = 0x7F0FFFFFu,
  TermCtrl = 0x100000u,
  TermAlt = 0x200000u,
  TermSuper = 0x300000u,
  TermShift = 0x400000u,
} TermKeyValue;

TermResponse term_raw_enable(void);
TermResponse term_raw_disable(void);

TermResponse term_altscreen_enable(void);
TermResponse term_altscreen_disable(void);

TermResponse term_flush(void);

TermResponse term_size(unsigned short* r, unsigned short* c);

TermResponse term_cursor_show(void);
TermResponse term_cursor_hide(void);
TermResponse term_cursor_pos(unsigned short* r, unsigned short* c);
TermResponse term_cursor_move(unsigned short r, unsigned short c);

static inline TermColor TermRGB(
    unsigned char r,
    unsigned char g,
    unsigned char b
) {
  TermColor result = r;
  result = result << 8;
  result |= g;
  result = result << 8;
  result |= b;
  return result;
}
static inline unsigned char term_rgb_red(TermColor rgb) {
  return (rgb >> 16) & 0xFF;
}
static inline unsigned char term_rgb_green(TermColor rgb) {
  return (rgb >> 8) & 0xFF;
}
static inline unsigned char term_rgb_blue(TermColor rgb) {
  return rgb & 0xFF;
}

TermResponse term_style_reset(void);
TermResponse term_color_fg(TermColor);
TermResponse term_color_bg(TermColor);

TermResponse term_clear(void);
TermResponse term_clear_line(void);
TermResponse term_clear_leading(void);
TermResponse term_clear_trailing(void);

TermResponse term_write(unsigned short r, unsigned short c, char const*);
TermResponse term_write_line(unsigned short r, char const*);
TermResponse term_write_cursor(char const*);

TermResponse term_read_key(TermKey*);


#endif // FI_TERM_H
