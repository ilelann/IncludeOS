#
# CMake script for the OS library
#

if (PROFILE)
  add_definitions(-DENABLE_PROFILERS)
endif()

set(SRCS
  version.cpp
)
set(LIBRARIES
    hal
    kernel
    util
    net
    fs
    posix
    virtio
    hw
)

if (NOT CMAKE_TESTING_ENABLED)
  list(APPEND LIBRARIES crt)
endif()

SET(OBJECTS)
#TODO make the subdirectory add return an LIST of target objects..
#needed to make multilevel dep work for eg.. net.
foreach(LIB ${LIBRARIES})
    add_subdirectory(${LIB})
    list(APPEND OBJECTS "$<TARGET_OBJECTS:${LIB}>" )
endforeach()

#if (CMAKE_TESTING_ENABLED)
#  list(APPEND SRCS
#    arch/${ARCH}/paging.cpp
#  )
#endif()

if (NOT VERSION)
  set(VERSION VERY_DIRTY)
endif()

FILE(WRITE ${CMAKE_CURRENT_BINARY_DIR}/version.h
  "#define OS_VERSION \"${VERSION}\"\n"
)

add_library(includeos_impl INTERFACE)
target_link_libraries(includeos_impl INTERFACE includeos_api
    #includeos_libcxx includeos_libc
)
target_include_directories(includeos_impl INTERFACE include)
target_compile_definitions(includeos_impl INTERFACE __includeos__)
target_link_options(includeos_impl INTERFACE -nostdlib)
target_compile_options(includeos_impl INTERFACE -c -nostdlibinc)

define_property(TARGET PROPERTY INCLUDEOS_PLATFORM_PRE_BSS_AREA BRIEF_DOCS "TODO" FULL_DOCS "TODO")

#TODO check if this is almost correct for platform userspace
#if (NOT CMAKE_TESTING_ENABLED)
  add_subdirectory(platform)
  add_subdirectory(arch)
  #add_subdirectory(drivers)
  #add_subdirectory(plugins)
  add_subdirectory(musl)
#endif()

#
# Installation
#
#set(CMAKE_INSTALL_MESSAGE LAZY) # to avoid spam
#install(TARGETS os DESTINATION lib)
##if (NOT CMAKE_TESTING_ENABLED)
##  configure_file(memdisk/empty.asm ${CMAKE_BINARY_DIR}/tools/memdisk/empty.asm)
##  configure_file(memdisk/memdisk.asm ${CMAKE_BINARY_DIR}/tools/memdisk/memdisk.asm)
##  configure_file(memdisk/memdisk.py ${CMAKE_BINARY_DIR}/tools/memdisk/memdisk.py)
##endif()
##TODO build ?
#install(DIRECTORY ${CMAKE_SOURCE_DIR}/src/memdisk/ DESTINATION tools/memdisk
#        FILES_MATCHING PATTERN "*.*")
#
#install(FILES service_name.cpp DESTINATION src)
