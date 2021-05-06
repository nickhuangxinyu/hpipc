#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <iostream>

using namespace std;

#define min(a, b) a > b ? b : a
template <typename T, unsigned long  CNT>
class FIFO_Queue {
 public:
   static_assert(CNT && !(CNT& (CNT - 1)), "CNT must be power of 2");

   FIFO_Queue() {
     k.in = k.out = 0;
     k.esize = sizeof(T);
     k.mask = CNT - 1;
   }
   ~FIFO_Queue() { __kfifo_free(&k);}

   bool push(const T& t) { return __kfifo_in(&k, &t, 1); }

   bool pop(T& t) { return __kfifo_out(&k, &t, 1); }

 private:
  struct __kfifo {
    unsigned int in, out, mask, esize;
    T data[CNT];
  };
  
  __kfifo k;

 private:
  static char* kmalloc(unsigned size){
    char *p = (char*)memalign(64, size);
    if (!p) return p;
    memset(p, 0, size);
    return p;
  }
    
  /* static inline char *kmalloc_array(size_t n, size_t size) {
    return size != 0 && n > SIZE_MAX / size ? NULL : kmalloc(n * size);
  }*/
  
  static inline unsigned int kfifo_unused(struct __kfifo *fifo){
    return (fifo->mask + 1) - (fifo->in - fifo->out);
  }
  
  void __kfifo_free(struct __kfifo *fifo){
    fifo->in = fifo->out = fifo->esize = fifo->mask = 0;
    // fifo->data = NULL;
  }
  
  /* static int kfifo_alloc(struct __kfifo* fifo, unsigned int size, size_t esize) {
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
  }*/
  
  /*int __kfifo_init(struct __kfifo *fifo, unsigned int size, size_t esize) {
    // size /= esize;
    cout << "before init size = " << size << endl;
    if (!is_power_of_2(size)) size = rounddown_pow_of_two(size);
    cout << "init size = " << size << endl;
    fifo->in = fifo->out = 0;
    fifo->esize = esize;
    fifo->data = NULL;
    if (size < 2) {
      fifo->mask = 0;
      return -1;
    }
    fifo->mask = size - 1;
    return 0;
  }*/
  
  static void kfifo_copy_in(struct __kfifo *fifo, const T *src, unsigned int len, unsigned int off) {
    unsigned int size = fifo->mask + 1;
    unsigned int esize = fifo->esize;
    
    off &= fifo->mask;
    if (esize != 1) {
      off *= esize;
      size *= esize;
      // len *= esize;
    }
    unsigned int l = min(len, size - off);
    cout << "size = " << size << endl;
    cout << "off = " << off << endl;
    cout << "len = " << len << endl;
    cout << "esize = " << esize << endl;
    cout << "l = " << l << endl;
    /* size = 32 off = 0 len = 1 esize = 4 l = 1 value=3
    */
    
    cout << "value=" << *(src) << endl;
    memcpy(fifo->data+off, src, l);
    cout << "finished fifo" << endl;
    memcpy(fifo->data, src + l, (len - l) * sizeof(T));
    asm volatile("" ::: "memory");
  }
    
  unsigned int __kfifo_in(struct __kfifo *fifo, const T* buf, unsigned int len) {
    unsigned int l = kfifo_unused(fifo);
    if (len > l) len = l;  // if len > unused, just push unused, enfull the queue
    kfifo_copy_in(fifo, buf, len, fifo->in);
    fifo->in += len;
    return len;
  }
    
  static void kfifo_copy_out(struct __kfifo *fifo, T* dst, unsigned int len, unsigned int off){
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
    
  unsigned int __kfifo_out_peek(struct __kfifo *fifo, T* buf, unsigned int len){
    unsigned int l;
    l = fifo->in - fifo->out;
    if (len > l) len = l;
    kfifo_copy_out(fifo, buf, len, fifo->out);
    return len;
  }
    
  unsigned int __kfifo_out(struct __kfifo *fifo, T* buf, unsigned int len){
    len = __kfifo_out_peek(fifo, buf, len);
    fifo->out += len;
    return len;
  }
};
