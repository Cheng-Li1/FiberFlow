#include "FiberFlow.h"
#include "settings.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define AMD64_RegsterPart 0xE

struct Fiber {
    // Part of the structure that used to store the register data
    uint64_t registerPart[AMD64_RegsterPart];
    // Function pointer
    uint64_t func;
    // Input data
    uint64_t data;
};

static thread_local Fiber_t active_fiber;
static void (*fiber_swap)(Fiber_t, Fiber_t) = 0;
static uint8_t init_fiber[sizeof(struct Fiber)];

/*  
    Those binary code save and restore the callee save registers which includes:
    %r10	Scratch/temporary, caller-saved	%r10d	%r10w	%r10b	
    %r11	Scratch/temporary, caller-saved	%r11d	%r11w	%r11b	
    %rsp	Stack pointer,  callee-saved	%esp	%sp	%spl	
    %rbx	Local variable, callee-saved	%ebx	%bx	%bl	
    %rbp	Local variable, callee-saved	%ebp	%bp	%bpl	
    %r12	Local variable, callee-saved	%r12d	%r12w	%r12b	
    %r13	Local variable, callee-saved	%r13d	%r13w	%r13b	
    %r14	Local variable, callee-saved	%r14d	%r14w	%r14b	
    %r15	Local variable, callee-saved	%r15d	%r15w	%r15b
*/
/* 
    Important note about how the "pop rax" and "jmp rax" work as intended:
    In x86-64 assembly, when a function is called (typically using the call instruction), 
    the CPU automatically pushes the return address onto the stack. The return address is 
    the address of the instruction immediately following the call instruction in the calling function.
 */
 // This line of code ask the compiler to put the function in text segment
section(text)
const unsigned char swap_function[4096] = {
  0x48, 0x89, 0x26,                /* mov [rsi],rsp    */
  0x48, 0x8b, 0x27,                /* mov rsp,[rdi]    */
  0x58,                                    /* pop rax          */
  0x48, 0x89, 0x6e, 0x08,     /* mov [rsi+ 8],rbp */
  0x48, 0x89, 0x5e, 0x10,  /* mov [rsi+16],rbx */
  0x4c, 0x89, 0x66, 0x18,  /* mov [rsi+24],r12 */
  0x4c, 0x89, 0x6e, 0x20,  /* mov [rsi+32],r13 */
  0x4c, 0x89, 0x76, 0x28,  /* mov [rsi+40],r14 */
  0x4c, 0x89, 0x7e, 0x30,  /* mov [rsi+48],r15 */
  0x48, 0x8b, 0x6f, 0x08,  /* mov rbp,[rdi+ 8] */
  0x48, 0x8b, 0x5f, 0x10,  /* mov rbx,[rdi+16] */
  0x4c, 0x8b, 0x67, 0x18,  /* mov r12,[rdi+24] */
  0x4c, 0x8b, 0x6f, 0x20,  /* mov r13,[rdi+32] */
  0x4c, 0x8b, 0x77, 0x28,  /* mov r14,[rdi+40] */
  0x4c, 0x8b, 0x7f, 0x30,  /* mov r15,[rdi+48] */
  0xff, 0xe0,                        /* jmp rax          */
};

static void Fiber_wrapper(Fiber_t handle) {
  void (*function)(void) = (void (*)(void))(handle->func);
  function();
  // Crash the program intentionally if entry point return
  __builtin_trap();
}

Fiber_t Fiber_init() {
  active_fiber = (void *)init_fiber;
  fiber_swap = (void (*)(Fiber_t, Fiber_t))swap_function;
  return active_fiber;
}

Fiber_t Fiber_active() {
  return active_fiber;
}

void Fiber_switch(Fiber_t handle) {
  register Fiber_t previous_fiber = active_fiber;
  fiber_swap(active_fiber = handle, previous_fiber);
}

Fiber_t Fiber_create(void* memory, uint64_t size, void (*function)(void)) {
  Fiber_t handle = (Fiber_t)(((((uint64_t)memory + size) & ~0xF) - sizeof(struct Fiber)) & ~0xF);
  
  uint64_t *p = (uint64_t*)(handle);            /* seek to top of stack */
  *--p = (uint64_t)0;                           /* crash if entrypoint returns(actually not needed as Fiber wrapper have trap inside it) */
  *--p = (uint64_t)Fiber_wrapper;               /* Would be used as rax, Fiber_t handle argument is passed through fiber_swap */
  ((uint64_t*)handle)[0] = (uint64_t)p;         /* stack pointer */
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
