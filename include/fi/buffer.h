#ifndef FI_BUFFER_H
#define FI_BUFFER_H

// Standard Library
#include <stddef.h>


typedef struct Buffer Buffer;

Buffer* buffer_alloc(void);
void buffer_free(Buffer* b);

size_t buffer_line_count(Buffer const* b);
void buffer_line_get(Buffer* b, size_t row, char* dest, size_t sz);
void buffer_line_insert(Buffer* b, size_t row, char const* s, size_t sz);
size_t buffer_line_size(Buffer const* b, size_t row);


#endif // FI_BUFFER_H
