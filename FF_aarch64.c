#include "FiberFlow.h"
#include "settings.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct Fiber {
    // Structure to store the registers data
    uint64_t registerPart[24];
    // Function pointer
    uint64_t func;
    // Input data
    uint64_t data;
};

static thread_local Fiber_t active_fiber;
static void (*fiber_swap)(Fiber_t, Fiber_t) = 0;

/* 
  ARM 64-bit architecture (AAPCS64)
  Caller-Saved Registers (Volatile)
  These registers are not required to be preserved across function calls. 
  If a function uses these registers and needs their values after a function call, it must save and restore them itself.

  General-Purpose Registers: X0 to X7, X9 to X15
  Floating-Point Registers: D0 to D7, D16 to D31
  SIMD Registers: V0 to V7, V16 to V31

  Callee-Saved Registers (Non-Volatile)
  These registers must be preserved across function calls. 
  If a function modifies these registers, it must save the original values and restore them before returning.

  General-Purpose Registers: X19 to X28
  Frame Pointer (Optional): X29 - It's optional because some procedures may use a frame pointer chain, while others may not.
  Link Register: X30 - It holds the return address.
  Stack Pointer: SP (X31) - Used as a stack pointer, and it’s implicit that functions should maintain stack integrity.
  Floating-Point Registers: D8 to D15
 */
// This line of code ask the compiler to put the function in text segment
section(text)
const uint32_t swap_function[1024] = {
  0x910003f0,  /* mov x16,sp           */
  0xa9007830,  /* stp x16,x30,[x1]     */
  0xa9407810,  /* ldp x16,x30,[x0]     */
  0x9100021f,  /* mov sp,x16           */
  0xa9015033,  /* stp x19,x20,[x1, 16] */
  0xa9415013,  /* ldp x19,x20,[x0, 16] */
  0xa9025835,  /* stp x21,x22,[x1, 32] */
  0xa9425815,  /* ldp x21,x22,[x0, 32] */
  0xa9036037,  /* stp x23,x24,[x1, 48] */
  0xa9436017,  /* ldp x23,x24,[x0, 48] */
  0xa9046839,  /* stp x25,x26,[x1, 64] */
  0xa9446819,  /* ldp x25,x26,[x0, 64] */
  0xa905703b,  /* stp x27,x28,[x1, 80] */
  0xa945701b,  /* ldp x27,x28,[x0, 80] */
  0xf900303d,  /* str x29,    [x1, 96] */
  0xf940301d,  /* ldr x29,    [x0, 96] */
  0x6d072428,  /* stp d8, d9, [x1,112] */
  0x6d472408,  /* ldp d8, d9, [x0,112] */
  0x6d082c2a,  /* stp d10,d11,[x1,128] */
  0x6d482c0a,  /* ldp d10,d11,[x0,128] */
  0x6d09342c,  /* stp d12,d13,[x1,144] */
  0x6d49340c,  /* ldp d12,d13,[x0,144] */
  0x6d0a3c2e,  /* stp d14,d15,[x1,160] */
  0x6d4a3c0e,  /* ldp d14,d15,[x0,160] */
  0xd61f03c0,  /* br x30               */
};

static void Fiber_wrapper(Fiber_t handle) {
  void (*function)(void) = (void (*)(void))(handle->func);
  function();
  // Crash the program intentionally if entry point return
  __builtin_trap();
}

void Fiber_init(Fiber_t memory) {
  active_fiber = memory;
  fiber_swap = (void (*)(Fiber_t, Fiber_t))swap_function;
}

Fiber_t Fiber_active() {
  return active_fiber;
}

void Fiber_switch(Fiber_t handle) {
  register Fiber_t previous_fiber = active_fiber;
  fiber_swap(active_fiber = handle, previous_fiber);
}

Fiber_t Fiber_create(void* memory, uint64_t size, void (*function)(void)) {
  if (memory == (void *)0 || size < MIN_STACK_SIZE) {
    return 0;
  }
  Fiber_t handle = (Fiber_t)((((uint64_t)memory + size) & ~0xF) - sizeof(struct Fiber));
  
  uint64_t* registers = (uint64_t *)handle;
  registers[0]  = (uint64_t)handle;              /* x16 (stack pointer) */
  registers[1]  = (uintptr_t)Fiber_wrapper;      /* x30 (link register) */
  registers[12] = (uint64_t)handle;              /* x29 (frame pointer) */

  handle->func = (uint64_t)function;

  return handle;
}

void Fiber_setargs(Fiber_t handle, void* data) {
    handle->data = (uint64_t)data;
}

void* Fiber_getargs(Fiber_t handle) {
    return (void*)handle->data;
}

#ifdef __cplusplus
}
#endif
