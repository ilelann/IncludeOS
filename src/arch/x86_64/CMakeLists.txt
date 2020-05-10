enable_language(ASM_NASM)

set(CMAKE_ASM_NASM_COMPILE_OBJECT "<CMAKE_ASM_NASM_COMPILER> -felf64 -o <OBJECT> <SOURCE>")

### x86_64 arch specific ###
set(ARCH_OBJECTS
  apic_asm.asm
  apic_longmode.asm
  arch_start.asm
  exceptions.asm
  interrupts.asm
  fiber_asm.asm
  threads.asm
  __syscall_entry.asm
  syscall_entry.cpp
  ist.cpp
  paging.cpp
  init_paging.cpp
)

get_property(PRE_BSS_AREA_SET TARGET includeos_platform PROPERTY INCLUDEOS_PLATFORM_PRE_BSS_AREA SET)
if (NOT ${PRE_BSS_AREA_SET})
  message(FATAL_ERROR "fix build, no PRE_BSS_AREA")
endif()
get_property(PRE_BSS_AREA TARGET includeos_platform PROPERTY INCLUDEOS_PLATFORM_PRE_BSS_AREA)

if (FOR_PRODUCTION)
  set(PROD_USE 0x2000)
else()
  set(PROD_USE 0x1000)
endif()

add_library(arch STATIC ${ARCH_OBJECTS})
target_link_libraries(arch PRIVATE
    includeos_impl
    includeos_libcxx
    includeos_libc
)

target_link_options(arch PUBLIC "LINKER:--defsym,PRE_BSS_AREA=${PRE_BSS_AREA}")
target_link_options(arch PUBLIC "LINKER:--defsym,__for_production_use=${PROD_USE}")
target_link_options(arch PUBLIC "LINKER:-T,${CMAKE_CURRENT_SOURCE_DIR}/linker.ld")

if (FOR_PRODUCTION)
  set(PROD_USE "--defsym __for_production_use=0x2000")
else()
  set(PROD_USE "--defsym __for_production_use=0x1000")
endif()


set_target_properties(arch PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(arch PROPERTIES INTERFACE_LINK_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld)
