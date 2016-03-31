
#define CAPACITY 128

volatile char c_buf[CAPACITY];
volatile unsigned head = 0, tail = 0;

/* Returns 1 if the buffer is empty, 0 otherwise */
int cir_empty() {
  return head == tail;
}

/* Returns 1 if the buffer is full (no more elements could fit), 0 otherwise */
int cir_full() {
  int result = 0;
  // TODO: Fill me in.
  if ((head+1) % CAPACITY == tail) result = 1;
  return result;
}

/* Add a byte to the buffer */
void cir_push(char elem) {
  if (cir_full()) return;

  c_buf[head] = elem;
  head = (head + 1) % CAPACITY;
  // TODO: Put elem in head position
  // TODO: Increment head
}


/* Remove a byte from the buffer (blocks until byte is available) */
char cir_pop() {
  if (cir_empty()) return 0;
  char elem = 0;
  /*while (cir_empty()) {[> Wait for a byte<]};*/
  
  
  elem = c_buf[tail];
  tail = (tail + 1) % CAPACITY;
  // TODO: Get element in tail position
  // TODO: Increment tail
  return elem;
}
