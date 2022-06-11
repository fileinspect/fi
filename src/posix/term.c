#include "fi/term.h"

// Standard Library
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// POSIX
#include <sys/ioctl.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>


#define CTRL_KEY(k) ((k) & 0x1f)

static struct termios orig_termios;

static void die(char const* s) {
  term_clear();

  if (s != NULL) {
    perror(s);
    exit(1);
  }
  exit(0);
}

static void disable_raw(void) {
  term_raw_disable();
}

TermResponse term_raw_enable(void) {
  if (tcgetattr(STDIN_FILENO, &orig_termios) == -1)
    die("tcgetattr");
  atexit(disable_raw);

  struct termios raw = orig_termios;
  raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  raw.c_oflag &= ~(OPOST);
  raw.c_cflag |= (CS8);
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 1;

  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
    die("tcsetattr");
  return TermSuccess;
}

TermResponse term_raw_disable(void) {
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
    die("tcsetattr");
  return TermSuccess;
}

static void disable_altscreen(void) {
  term_altscreen_disable();
}

TermResponse term_altscreen_enable(void) {
  if (write(STDOUT_FILENO, "\033[?1049h", 11) != 11)
    return TermFailure;
  atexit(disable_altscreen);
  return TermSuccess;
}

TermResponse term_altscreen_disable(void) {
  if (write(STDOUT_FILENO, "\033[?1049l", 11) != 11)
    return TermFailure;
  return TermSuccess;
}

TermResponse term_flush(void) {
  fflush(stdout);
  return TermSuccess;
}

TermResponse term_size(unsigned short* r, unsigned short* c) {
  struct winsize ws;

  if (!r && !c) return TermSuccess;

  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
    if (term_cursor_move(999, 999) == TermFailure)
      return TermFailure;
    if (term_cursor_pos(r, c) == TermFailure)
      return TermFailure;
    else if (r) *r += 1;
    else if (c) *c += 1;
    return TermSuccess;
  }
  else {
    if (c) *c = ws.ws_col;
    if (r) *r = ws.ws_row;
    return TermSuccess;
  }
}

TermResponse term_cursor_show(void) {
  if (write(STDOUT_FILENO, "\x1b[?25h", 6) != 6)
    return TermFailure;
  return TermSuccess;
}

TermResponse term_cursor_hide(void) {
  if (write(STDOUT_FILENO, "\x1b[?25l", 6) != 6)
    return TermFailure;
  return TermSuccess;
}

TermResponse term_cursor_pos(unsigned short* r, unsigned short* c) {
  if (!r && !c) return TermSuccess;

  char buf[32];
  unsigned int i = 0;

  if (write(STDOUT_FILENO, "\x1b[6n", 4) != 4)
    return TermFailure;

  printf("\r\n");
  while (i < sizeof(buf) - 1) {
    if (read(STDIN_FILENO, &buf[i], 1) != 1) break;
    if (buf[i] == 'R') break;
    ++i;
  }
  buf[i] = 0;

  int rows, cols;
  if (buf[0] != '\x1b' || buf[1] != '[')
    return TermFailure;
  if (sscanf(&buf[2], "%d;%d", &rows, &cols) != 2)
    return TermFailure;

  if (r) *r = rows - 1;
  if (c) *c = cols - 1;

  return TermSuccess;
}

TermResponse term_cursor_move(unsigned short r, unsigned short c) {
  char buf[15];
  sprintf(buf, "\x1b[%d;%dH", r + 1, c + 1);
  if (write(STDOUT_FILENO, buf, strlen(buf)) != (ssize_t)strlen(buf))
    return TermFailure;
  return TermSuccess;
}

TermResponse term_cursor_home(void) {
  if (write(STDOUT_FILENO, "\x1b[H", 3) != 3)
    return TermFailure;
  return TermSuccess;
}

TermResponse term_style_reset(void) {
  if (fwrite("\x1b[0m", 1, 4, stdout) != 4)
    return TermFailure;
  return TermSuccess;
}

TermResponse term_color_fg(TermColor c) {
  char buf[20];
  if (c >= 256) {
    sprintf(
        buf,
        "\x1b[38;2;%d;%d;%dm",
        term_rgb_red(c),
        term_rgb_green(c),
        term_rgb_blue(c)
    );
  }
  else if (c >= 16) {
    sprintf(buf, "\x1b[38;5;%dm", c);
  }
  else if (c >= 8) {
    sprintf(buf, "\x1b[%dm", 82 + c);
  }
  else {
    sprintf(buf, "\x1b[%dm", 30 + c);
  }
  if (fwrite(buf, 1, strlen(buf), stdout) != strlen(buf))
    return TermFailure;
  return TermSuccess;
}

TermResponse term_color_bg(TermColor c) {
  char buf[20];
  if (c >= 256) {
    sprintf(
        buf,
        "\x1b[48;2;%d;%d;%dm",
        term_rgb_red(c),
        term_rgb_green(c),
        term_rgb_blue(c)
    );
  }
  else if (c >= 16) {
    sprintf(buf, "\x1b[48;5;%dm", c);
  }
  else if (c >= 8) {
    sprintf(buf, "\x1b[%dm", 92 + c);
  }
  else {
    sprintf(buf, "\x1b[%dm", 40 + c);
  }
  if (fwrite(buf, 1, strlen(buf), stdout) != strlen(buf))
    return TermFailure;
  return TermSuccess;
}

TermResponse term_clear(void) {
  if (write(STDOUT_FILENO, "\x1b[?25l", 6) != 6)
    return TermFailure;
  if (write(STDOUT_FILENO, "\x1b[2J", 4) != 4)
    return TermFailure;
  if (write(STDOUT_FILENO, "\x1b[H", 3) != 3)
    return TermFailure;
  if (write(STDOUT_FILENO, "\x1b[?25h", 6) != 6)
    return TermFailure;
  return TermSuccess;
}

TermResponse term_clear_line(void) {
  if (write(STDOUT_FILENO, "\x1b[2K", 4) != 4)
    return TermFailure;
  return TermSuccess;
}

TermResponse term_clear_leading(void) {
  if (write(STDOUT_FILENO, "\x1b[1K", 4) != 4)
    return TermFailure;
  return TermSuccess;
}

TermResponse term_clear_trailing(void) {
  if (write(STDOUT_FILENO, "\x1b[0K", 4) != 4)
    return TermFailure;
  return TermSuccess;
}

TermResponse term_write(unsigned short r, unsigned short c, char const* msg) {
  if (!msg) return TermSuccess;

  char buf[15];
  sprintf(buf, "\x1b[%d;%dH", r + 1, c + 1);
  if (fwrite(buf, 1, strlen(buf), stdout) != strlen(buf))
    return TermFailure;

  if (fwrite(msg, 1, strlen(msg), stdout) != strlen(msg))
    return TermFailure;

  return TermSuccess;
}

TermResponse term_write_line(unsigned short r, char const* msg) {
  if (!msg) return TermSuccess;

  char buf[11];
  sprintf(buf, "\x1b[%d;0H", r + 1);
  if (fwrite(buf, 1, strlen(buf), stdout) != strlen(buf))
    return TermFailure;

  if (fwrite(msg, 1, strlen(msg), stdout) != strlen(msg))
    return TermFailure;

  if (fwrite("\x1b[0K", 1, 4, stdout) != 4)
    return TermFailure;

  return TermSuccess;
}

TermResponse term_write_cursor(char const* msg) {
  if (!msg) return TermSuccess;

  if (fwrite(msg, 1, strlen(msg), stdout) != strlen(msg))
    return TermFailure;

  return TermSuccess;
}

TermResponse term_read_key(TermKey* dest) {
  int nread;
  char c;
  while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
    if (nread == -1 && errno != EAGAIN) return TermFailure;
  }

  if (c == '\x1b') {
    char seq[3];
    seq[0] = 0;
    seq[1] = 0;
    seq[2] = 0;

    if (read(STDIN_FILENO, &seq[0], 1) != 1) {
      // *dest = '\x1b';
      return TermFailure; // TODO: Check if this is failure
    }
    if (read(STDIN_FILENO, &seq[1], 1) != 1) {
      *dest = '\x1b';
      return TermFailure; // TODO: Check if this is failure
    }

    if (seq[0] == '[') {
      if (seq[1] >= '0' && seq[1] <= '9') {
        if (read(STDIN_FILENO, &seq[2], 1) != 1) *dest = '\x1b';
        if (seq[2] == '~') {
          switch (seq[1]) {
            case '1':
              *dest = TermHome;
              return TermSuccess;
            case '3':
              *dest = TermDelete;
              return TermSuccess;
            case '4':
              *dest = TermEnd;
              return TermSuccess;
            case '5':
              *dest = TermPageUp;
              return TermSuccess;
            case '6':
              *dest = TermPageDown;
              return TermSuccess;
            case '7':
              *dest = TermHome;
              return TermSuccess;
            case '8':
              *dest = TermEnd;
              return TermSuccess;
          }
        }
      }
      else {
        switch (seq[1]) {
          case 'A':
            *dest = TermArrowUp;
            return TermSuccess;
          case 'B':
            *dest = TermArrowDown;
            return TermSuccess;
          case 'C':
            *dest = TermArrowRight;
            return TermSuccess;
          case 'D':
            *dest = TermArrowLeft;
            return TermSuccess;
          case 'H':
            *dest = TermHome;
            return TermSuccess;
          case 'F':
            *dest = TermEnd;
            return TermSuccess;
        }
      }
    }
    else if (seq[0] == 'O') {
      switch (seq[1]) {
        case 'H':
          *dest = TermHome;
          return TermSuccess;
        case 'F':
          *dest = TermEnd;
          return TermSuccess;
      }
    }

    *dest = '\x1b';
    return TermFailure; // TODO: Check if this is failure
  }
  else {
    switch (c) {
      case CTRL_KEY('q'):
        *dest = TermCtrl | 'q';
        return TermSuccess;
    }
    *dest = c;
    return TermSuccess;
  }
}
