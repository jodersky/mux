#include <collection/rbuffer.h>

void rbuffer_init(struct rbuffer_t* rb, rbuffer_byte_t* address, size_t size) {
  rb->address = address;
  rb->size = size;
  rb->head=0;
  rb->tail=0;
}
