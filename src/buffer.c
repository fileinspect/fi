#include "fi/buffer.h"

// Standard Library
#include <stdlib.h>
#include <string.h>


struct Buffer {
  size_t line_count;
  size_t* line_sizes;
  char** lines;
};

Buffer* buffer_alloc(void) {
  Buffer* result = malloc(sizeof(Buffer));
  result->line_count = 0;
  result->line_sizes = NULL;
  result->lines = NULL;
  return result;
}

void buffer_free(Buffer* b) {
  if (!b) return;
  free(b->line_sizes);
  for (size_t i = 0; i < b->line_count; ++i) {
    free(b->lines[i]);
  }
  free(b->lines);
  free(b);
}

size_t buffer_line_count(Buffer const* b) {
  if (!b) return 0;
  return b->line_count;
}

void buffer_line_get(Buffer* b, size_t row, char* dest, size_t sz) {
  if (!b || !dest || row >= b->line_count) return;
  size_t cpy_amnt = b->line_sizes[row] < sz ? b->line_sizes[row] : sz;
  memcpy(dest, b->lines[row], cpy_amnt);
}

void buffer_line_insert(Buffer* b, size_t row, char const* s, size_t sz) {
  if (!b || !s || row > b->line_count) return;

  void* realloc_result = realloc(
      b->lines,
      sizeof(char*) * (b->line_count + 1)
  );
  if (!realloc_result) return;
  else b->lines = realloc_result;

  realloc_result = realloc(
      b->line_sizes,
      sizeof(size_t) * (b->line_count + 1)
  );
  if (!realloc_result) return;
  else b->line_sizes = realloc_result;

  if (row < b->line_count) {
    memmove(
        &b->lines[row + 1],
        &b->lines[row],
        sizeof(char*) * (b->line_count - row)
    );
    memmove(
        &b->line_sizes[row + 1],
        &b->line_sizes[row],
        sizeof(size_t) * (b->line_count - row)
    );
  }

  b->lines[row] = malloc(sizeof(char*) * sz);
  memcpy(b->lines[row], s, sz);
  b->line_sizes[row] = sz;
  b->line_count++;
}

size_t buffer_line_size(Buffer const* b, size_t row) {
  if (!b || row >= b->line_count) return 0;
  return b->line_sizes[row];
}
