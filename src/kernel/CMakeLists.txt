enable_language(ASM_NASM)

set(CMAKE_ASM_NASM_COMPILE_OBJECT "<CMAKE_ASM_NASM_COMPILER> -felf64 -o <OBJECT> <SOURCE>")

set(SRCS
    block.cpp
    cpuid.cpp
    elf.cpp
    events.cpp
    # TODO requires <atomic>
    #fiber.cpp
    memmap.cpp
    multiboot.cpp
    os.cpp
    panic.cpp
    profile.cpp
    service_stub.cpp
    #scoped_profiler.cpp
    smp_common.cpp
    smp_utils.cpp
    terminal.cpp
    timers.cpp
    threads.cpp
    #tls.cpp
    rng.cpp
    vga.cpp
    context.cpp
    #context_asm.asm
    )

set(MOCKABLE_SRCS
  #  rdrand.cpp
    heap.cpp
    kernel.cpp
    liveupdate.cpp
    rtc.cpp
    system_log.cpp
  )

if("${ARCH}" STREQUAL "x86_64" OR "${ARCH}" STREQUAL "i686")
  list(APPEND SRCS
    context_asm.asm
    scoped_profiler.cpp
  )
endif()

add_library(kernel OBJECT ${SRCS} ${MOCKABLE_SRCS})
target_link_libraries(kernel PRIVATE
    includeos_impl
    includeos_libcxx
    includeos_libc
)

add_library(kernel_mockable OBJECT ${SRCS})
target_link_libraries(kernel_mockable PRIVATE
    includeos_impl
    includeos_libcxx
    includeos_libc
    )
