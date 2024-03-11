#ifndef FiberFlow
#define FiberFlow
#endif

#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>
typedef struct Fiber* Fiber_t;

Fiber_t Fiber_init();
Fiber_t Fiber_active();
void Fiber_switch(Fiber_t handle);
Fiber_t Fiber_create(void*, uint64_t, void (*)(void));
void Fiber_setargs(Fiber_t handle, void* data);
void* Fiber_getargs(Fiber_t handle);

#ifdef __cplusplus
}
#endif

/*
Simple tutorial and test
#include <stdio.h>
#include <stdlib.h>
#include "FiberFlow/FiberFlow.h"

Fiber_t main_thread;
Fiber_t test_thread1;
Fiber_t test_thread2;
Fiber_t test_thread3;
Fiber_t test_thread4;

void test() {
    printf("Test1 for libco works fine!\n");
    printf("Now we give arguments for test 2\n");
    char* string = "Test2 for libco works fine!";
    Fiber_setargs(test_thread2, string);
    Fiber_switch(test_thread2);
}

void test2() {
    Fiber_t active = Fiber_active();
    if (active != test_thread2) {
        printf("Problems here\n");
    }
    char* string = Fiber_getargs(active);
    printf("%s\n", string);
    Fiber_switch(test_thread3);
}

void test3() {
    uint64_t stack_location;
    printf("Despite have unaligned input memory, our stack is at 0x%lx\n", (uint64_t)(&stack_location));
    Fiber_switch(test_thread4);
}

void test4() {
    printf("Should put stack overflow tests here~\n");
    printf("Going back to main thread\n");
    Fiber_switch(main_thread);
}

int main() {
    printf("Begin FiberFlow Tests!\n");
    void *stack1 = malloc(0x2000);
    void *stack2 = malloc(0x2000);
    void *stack3 = malloc(0x2000);
    void *stack4 = malloc(0x2000);
    main_thread = malloc(AMD64_RegsterPart);
    Fiber_init(main_thread);
    test_thread1 = Fiber_create(stack1, 0x2000, test);
    test_thread2 = Fiber_create(stack2, 0x2000, test2);
    // Intentionally put in misaligned value for stack
    test_thread3 = Fiber_create(stack3 + 1, 5093, test3);
    // I want do stack overflow task here but do not want to set up guard page in Linux
    test_thread4 = Fiber_create(stack4, 0x2000, test4);
    Fiber_switch(test_thread1);
    printf("Ahhh, I am back!\n");
    free(stack1);
    free(stack2);
    free(stack3);
    free(stack4);
    return 0;
}
*/