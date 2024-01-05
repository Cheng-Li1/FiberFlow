#ifndef FiberFlow
#define FiberFlow
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define MIN_STACK_SIZE 0x1000

#include <stdint.h>
typedef struct Fiber* Fiber_t;

Fiber_t Fiber_active();
void Fiber_switch(Fiber_t handle);
void Fiber_init(Fiber_t memory);
Fiber_t Fiber_create(void*, uint64_t, void (*)(void*), void*);

#ifdef __cplusplus
}
#endif
