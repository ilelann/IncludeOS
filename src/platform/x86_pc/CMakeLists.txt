enable_language(ASM_NASM)

set(CMAKE_ASM_NASM_COMPILE_OBJECT "<CMAKE_ASM_NASM_COMPILER> -felf64 -o <OBJECT> <SOURCE>")

set(PLATFORM_TARGET ${ARCH}_pc)
### x86 PC specific ###
set(X86_PC_OBJECTS
  acpi.cpp
  apic.cpp
  apic_timer.cpp
  apic_revenant.cpp
  clocks.cpp
  cmos.cpp
  cmos_clock.cpp
  cpu_freq_sampling.cpp
  gdt.cpp
  idt.cpp
  init_libc.cpp
  ioapic.cpp
  kernel_start.cpp
  os.cpp
  pic.cpp
  pit.cpp
  platform.cpp
  rand.cpp
  sanity_checks.cpp
  serial1.cpp
  smbios.cpp
  # TODO requires <thread>
  #smp.cpp
  softreset.cpp
  start.asm
  ### KVM features ###
  ../kvm/kvmclock.cpp
  ../kvm/pv_eoi.cpp
  )

#add_custom_command(
#	OUTPUT apic_boot.o
#	COMMAND ${CMAKE_ASM_NASM_COMPILER} -f bin -o apic_boot.bin ${CMAKE_CURRENT_SOURCE_DIR}/apic_boot.asm
#	COMMAND ${CMAKE_OBJCOPY} -I binary -O ${OBJCOPY_TARGET} -B i386 apic_boot.bin apic_boot.o
#	DEPENDS apic_boot.asm
#)

add_library(${PLATFORM_TARGET} STATIC ${X86_PC_OBJECTS})
#target_sources(${PLATFORM_TARGET} apic_boot.o)
target_link_libraries(${PLATFORM_TARGET} PRIVATE
    includeos_impl
    includeos_libc
    includeos_libcxx
)

#target_compile_definitions(${PLATFORM_TARGET} PRIVATE -DPLATFORM_x86_pc)
set_target_properties(${PLATFORM_TARGET} PROPERTIES INCLUDEOS_PLATFORM_PRE_BSS_AREA 0x0)
set_target_properties(${PLATFORM_TARGET}
  PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/platform
  ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/platform
  ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/platform
)

add_library(includeos_platform ALIAS ${PLATFORM_TARGET})

# disable sanitizers on kernel_start and others

#add_subdirectory(boot)
#set_target_properties(${PLATFORM_TARGET} PROPERTIES LINKER_LANGUAGE CXX)
install(TARGETS ${PLATFORM_TARGET} DESTINATION platform)
