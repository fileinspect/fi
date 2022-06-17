/* fi.c */

// Standard Library
#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// fi
#include "fi/buffer.h"
#include "fi/term.h"


#define FI_VERSION "0.0.1"
#define FI_TAB_STOP 2


typedef struct EditorConfig {
  int cx, cy;
  int rx;
  int rowoff;
  int coloff;
  unsigned short screenrows;
  unsigned short screencols;
  int numrows;
  char* filename;
  char statusmsg[80];
  time_t statusmsg_time;
} EditorConfig;

typedef struct ABuf {
  char *b;
  int len;
} ABuf;

#define ABUF_INIT { NULL, 0 }

EditorConfig E;
Buffer* buffer = NULL;

void die(char const* s) {
  term_clear();

  if (s != NULL) {
    perror(s);
    exit(1);
  }
  exit(0);
}

void editorScroll() {
  E.rx = 0;
  if (E.cy < E.numrows) {
    E.rx = E.cx;
  }

  if (E.cy < E.rowoff) {
    E.rowoff = E.cy;
  }
  if (E.cy >= E.rowoff + E.screenrows) {
    E.rowoff = E.cy - E.screenrows + 1;
  }
  if (E.rx < E.coloff) {
    E.coloff = E.cx;
  }
  if (E.rx >= E.coloff + E.screencols) {
    E.coloff = E.cx - E.screencols + 1;
  }
}

void editorDrawRows(void) {
  for (int row = 0; row < E.screenrows; ++row) {
    int filerow = row + E.rowoff;
    if (filerow >= E.numrows) {
      if (E.numrows == 0 && row == E.screenrows / 3) {
        char welcome[80];
        int welcomelen = snprintf(
            welcome,
            sizeof(welcome),
            "fi editor -- version %s",
            FI_VERSION
        );
        if (welcomelen > E.screencols) welcomelen = E.screencols;
        int padding = (E.screencols - welcomelen) / 2;
        if (padding) term_write(row, 0, "~");
        for (int j = 1; j < padding; ++j) {
          term_write(row, j, " ");
        }
        term_write(row, padding, welcome);
        term_clear_trailing();
      } else {
        term_write_line(row, "~");
      }
    } else {
      size_t len = buffer_line_size(buffer, filerow);
      if (len < (size_t)E.coloff) len = 0;
      else len -= E.coloff;
      if (len > E.screencols) len = E.screencols;
      char* buf = malloc(sizeof(char) * (len + 1));
      buffer_line_get(buffer, filerow, buf, len);
      buf[len] = 0;
      term_write_line(row, buf);
    }
  }
}

void editorDrawStatusBar(void) {
  term_color_fg(TermBlack);
  term_color_bg(TermWhite);
  char status[80], rstatus[80];
  int len = snprintf(status, sizeof(status), "%.20s - %d lines",
      E.filename ? E.filename : "[No Name]", E.numrows);
  int rlen = snprintf(rstatus, sizeof(rstatus), "%d/%d", E.cy + 1, E.numrows);
  if (len > E.screencols) len = E.screencols;
  term_write(E.screenrows, 0, status);

  while (len < E.screencols) {
    if (E.screencols - len == rlen) {
      term_write(E.screenrows, len, rstatus);
      break;
    }
    else {
      term_write(E.screenrows, len, " ");
      ++len;
    }
  }
  term_style_reset();
}

void editorDrawMessageBar(void) {
  int msglen = strlen(E.statusmsg);
  if (msglen > E.screencols) msglen = E.screencols;
  if (msglen && time(NULL) - E.statusmsg_time < 5)
    term_write_line(E.screenrows + 1, E.statusmsg);
}

void editorRefreshScreen() {
  editorScroll();

  term_cursor_hide();
  term_cursor_move(0, 0);

  editorDrawRows();
  editorDrawStatusBar();
  editorDrawMessageBar();

  term_flush();

  term_cursor_move(E.cy - E.rowoff, E.rx - E.coloff);
  term_cursor_show();
}

void editorSetStatusMessage(char const* fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(E.statusmsg, sizeof(E.statusmsg), fmt, ap);
  va_end(ap);
  E.statusmsg_time = time(NULL);
}

void editorMoveCursor(TermKey key) {
  size_t row_size = buffer_line_size(buffer, E.cy);

  switch (key) {
    case TermArrowLeft:
      if (E.cx != 0) --E.cx;
      break;
    case TermArrowRight:
      if ((size_t)E.cx < row_size) ++E.cx;
      break;
    case TermArrowUp:
      if (E.cy != 0) --E.cy;
      break;
    case TermArrowDown:
      if (E.cy < E.numrows) ++E.cy;
      break;
  }

  row_size = buffer_line_size(buffer, E.cy);
  if ((size_t)E.cx > row_size) {
    E.cx = row_size;
  }
}

void editorProcessKeypress() {
  TermKey c;
  if (term_read_key(&c)) return;

  switch (c) {
    case TermCtrl | 'q':
      term_clear();
      exit(0);
      break;

    case TermHome:
      E.cx = 0;
      break;
    case TermEnd:
      if (E.cy < E.numrows)
        E.cx = buffer_line_size(buffer, E.cy);
      break;

    case TermPageUp:
    case TermPageDown:
      {
        if (c == TermPageUp) {
          E.cy = E.rowoff;
        } else if (c == TermPageDown) {
          E.cy = E.rowoff + E.screenrows - 1;
          if (E.cy > E.numrows) E.cy = E.numrows;
        }

        int times = E.screenrows;
        while (times--) {
          editorMoveCursor(c == TermPageUp ? TermArrowUp : TermArrowDown);
        }
      }
      break;

    case TermArrowUp:
    case TermArrowDown:
    case TermArrowLeft:
    case TermArrowRight:
      editorMoveCursor(c);
      break;
  }
}

void abFree(ABuf *ab) {
  free(ab->b);
}

void initEditor() {
  E.cx = 0;
  E.cy = 0;
  E.rx = 0;
  E.rowoff = 0;
  E.coloff = 0;
  E.numrows = 0;
  E.filename = NULL;
  E.statusmsg[0] = 0;
  E.statusmsg_time = 0;

  buffer = buffer_alloc();

  if (term_size(&E.screenrows, &E.screencols) == TermFailure)
    die("term_size");
  E.screenrows -= 2;
}

void editorOpen(char const* filename) {
  free(E.filename);
  E.filename = malloc(strlen(filename) + 1);
  strcpy(E.filename, filename);

  FILE *fp = fopen(filename, "r");
  if (!fp) die("fopen");

  char* line = NULL;
  size_t linecap = 0;
  long int linelen;

  while ((linelen = getline(&line, &linecap, fp)) != -1) {
    while (linelen > 0 && (line[linelen - 1] == '\n' ||
                           line[linelen - 1] == '\r'))
      linelen--;
    buffer_line_insert(buffer, E.numrows++, line, linelen);
  }
  free(line);
  fclose(fp);
}

int main(int argc, char *argv[]) {
  term_raw_enable();
  term_altscreen_enable();
  initEditor();
  if (argc >= 2) {
    editorOpen(argv[1]);
  }

  editorSetStatusMessage("HELP: Ctrl-Q = quit");

  while (1) {
    editorRefreshScreen();
    editorProcessKeypress();
  }

  buffer_free(buffer);
  buffer = NULL;

  term_altscreen_disable();
  term_raw_disable();

  return 0;
}
