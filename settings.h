#if defined(FiberFlow)

#if defined(FiberFlow)
  #if defined(FiberFlow_MP)
    #define thread_local __thread
  #else
    #define thread_local
  #endif
#endif

#if __STDC_VERSION__ >= 201112L
  #include <stdalign.h>
#else
  #define alignas(bytes)
#endif

#define section(name) __attribute__((section("." #name "#")))

#endif