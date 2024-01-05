#if defined(__clang__)

  /* placing code in section(text) does not mark it executable with Clang. */
  #undef  FiberFlow_MPROTECT
  #define FiberFlow_MPROTECT
#endif

#if defined(__clang__) || defined(__GNUC__)
  #if defined(__amd64__)
    #include "FF_amd64.c"
  #elif defined(__aarch64__)
    #include "FF_aarch64.c"
  #else
    #error "FiberFlow: unsupported processor, compiler or operating system"
  #endif
#else
  #error "FiberFlow: unsupported compiler"
#endif