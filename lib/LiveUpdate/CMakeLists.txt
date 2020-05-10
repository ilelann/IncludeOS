
option(PROFILE "Compile with startup profilers" OFF)
if (PROFILE)
  add_definitions(-DENABLE_PROFILERS)
endif()

set(TRIPLE "${ARCH}-pc-linux-elf")
set(CMAKE_CXX_COMPILER_TARGET ${TRIPLE})
set(CMAKE_C_COMPILER_TARGET ${TRIPLE})

add_custom_command(
  PRE_BUILD
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/hotswap64.bin
	COMMAND ${CMAKE_ASM_NASM_COMPILER} -f bin -o ${CMAKE_CURRENT_BINARY_DIR}/hotswap64.bin ${CMAKE_CURRENT_SOURCE_DIR}/src/hotswap64.asm
  COMMENT "Building hotswap binary"
  DEPENDS src/hotswap64.asm
)
add_custom_target(hotswap64 DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/hotswap64.bin)


set(SRCS
  src/storage.cpp
  src/partition.cpp
  src/update.cpp
  src/resume.cpp
  src/rollback.cpp
  src/elfscan.cpp
  src/serialize_tcp.cpp
)
if (NOT CMAKE_TESTING_ENABLED)
  list(APPEND SRCS
    src/hotswap.cpp
    # TODO CMake seems to want to "C++"-compile this ...
    #src/hotswap64_blob.asm
  )
  if (${ARCH} STREQUAL "x86_64")
    list(APPEND SRCS src/serialize_s2n.cpp)
  endif()
endif()


# LiveUpdate static library
add_library(liveupdate STATIC ${SRCS} )
if (NOT CMAKE_TESTING_ENABLED)
  add_dependencies(liveupdate hotswap64)
endif()
set_target_properties(liveupdate PROPERTIES PUBLIC_HEADER "include/liveupdate;include/liveupdate.hpp")
#TODO GCC does not like nostdlibinc
target_compile_options(liveupdate PRIVATE $<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CXX_COMPILER_ID:AppleClang,Clang>>:-nostdlib -nostdlibinc>)
target_compile_options(liveupdate PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-Wall -Wextra -fstack-protector>)
target_compile_options(liveupdate PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-ffunction-sections -fdata-sections>)
target_include_directories(liveupdate PRIVATE include)
target_compile_options(liveupdate PRIVATE -Wno-unused-variable)
target_link_libraries(liveupdate PRIVATE s2n includeos_impl)
INSTALL(TARGETS liveupdate
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        PUBLIC_HEADER DESTINATION include
)
