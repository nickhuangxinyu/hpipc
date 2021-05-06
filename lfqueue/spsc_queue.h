#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>

#define min(a, b) a > b ? b : a

unsigned long roundup_pow_of_two(unsigned long n) {
  return n;
}
  
unsigned long rounddown_pow_of_two(unsigned long n) {
  return n;
}
  
int is_power_of_2(unsigned long n) {
  return n;
}

struct __kfifo {
  unsigned int in, out, mask, esize;
  void* data;
};

static void *kmalloc(unsigned size){
  void *p = memalign(64, size);
  if (!p) return p;
  memset(p, 0, size);
  return p;
}
  
static inline void *kmalloc_array(size_t n, size_t size) {
  return size != 0 && n > SIZE_MAX / size ? NULL : kmalloc(n * size);
}

static inline unsigned int kfifo_unused(struct __kfifo *fifo){
  return (fifo->mask + 1) - (fifo->in - fifo->out);
}

void __kfifo_free(struct __kfifo *fifo){
  if (fifo->data) free(fifo->data);
  fifo->in = fifo->out = fifo->esize = fifo->mask = 0;
  fifo->data = NULL;
}

static int kfifo_alloc(struct __kfifo* fifo, unsigned int size, size_t esize) {
  size = roundup_pow_of_two(size);
  fifo->in = 0;
  fifo->out = 0;
  fifo->esize = esize;
  if (size < 2) {
    fifo->data = NULL;
    fifo->mask = 0;
    return -1;
  }
  fifo->data = kmalloc_array(esize, size);
  if (!fifo->data) {
    fifo->mask = 0;
    return -2;
  }
  fifo->mask = size - 1;
  return 0;
}

int __kfifo_init(struct __kfifo *fifo, void *buffer, unsigned int size, size_t esize) {
  size /= esize;
  if (!is_power_of_2(size)) size = rounddown_pow_of_two(size);
  fifo->in = fifo->out = 0;
  fifo->esize = esize;
  fifo->data = buffer;
  if (size < 2) {
    fifo->mask = 0;
    return -1;
  }
  fifo->mask = size - 1;
  return 0;
}

static void kfifo_copy_in(struct __kfifo *fifo, const void *src, unsigned int len, unsigned int off) {
  unsigned int size = fifo->mask + 1;
  unsigned int esize = fifo->esize;
  unsigned int l;
  
  off &= fifo->mask;
  if (esize != 1) {
    off *= esize;
    size *= esize;
    len *= esize;
  }
  l = min(len, size - off);
  
  memcpy(fifo->data + off, src, l);
  memcpy(fifo->data, src + l, len - l);
  asm volatile("" ::: "memory");
}
  
unsigned int __kfifo_in(struct __kfifo *fifo, const void *buf, unsigned int len) {
  unsigned int l;
  l = kfifo_unused(fifo);
  if (len > l) len = l;
  kfifo_copy_in(fifo, buf, len, fifo->in);
  fifo->in += len;
  return len;
}
  
static void kfifo_copy_out(struct __kfifo *fifo, void *dst, unsigned int len, unsigned int off){
  unsigned int size = fifo->mask + 1;
  unsigned int esize = fifo->esize;
  unsigned int l;
  
  off &= fifo->mask;
  if (esize != 1) {
    off *= esize;
    size *= esize;
    len *= esize;
  }
  l = min(len, size - off);
  memcpy(dst, fifo->data + off, l);
  memcpy(dst + l, fifo->data, len - l);
  asm volatile("" ::: "memory");
}
  
unsigned int __kfifo_out_peek(struct __kfifo *fifo, void *buf, unsigned int len){
  unsigned int l;
  l = fifo->in - fifo->out;
  if (len > l) len = l;
  kfifo_copy_out(fifo, buf, len, fifo->out);
  return len;
}
  
unsigned int __kfifo_out(struct __kfifo *fifo, void *buf, unsigned int len){
  len = __kfifo_out_peek(fifo, buf, len);
  fifo->out += len;
  return len;
}
