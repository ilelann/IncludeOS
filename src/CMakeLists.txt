#
# CMake script for the OS library
#

if (PROFILE)
  add_definitions(-DENABLE_PROFILERS)
endif ()

add_library(includeos_impl INTERFACE)
target_link_libraries(includeos_impl INTERFACE includeos_api)
target_include_directories(includeos_impl INTERFACE include)
target_compile_definitions(includeos_impl INTERFACE __includeos__)

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

#if (NOT CMAKE_TESTING_ENABLED)
#  list(APPEND LIBRARIES crt)
#endif()

set(OBJECTS)
#TODO make the subdirectory add return an LIST of target objects..
#needed to make multilevel dep work for eg.. net.
foreach (LIB ${LIBRARIES})
  add_subdirectory(${LIB})
  list(APPEND OBJECTS "$<TARGET_OBJECTS:${LIB}>")
endforeach ()

add_library(os STATIC
    version.cpp
    $<TARGET_OBJECTS:hal>
    $<TARGET_OBJECTS:kernel>
    $<TARGET_OBJECTS:util>
    $<TARGET_OBJECTS:net>
    $<TARGET_OBJECTS:fs>
    $<TARGET_OBJECTS:posix>
    $<TARGET_OBJECTS:virtio>
    $<TARGET_OBJECTS:hw>
    )
target_link_libraries(os PRIVATE includeos_impl includeos_libcxx includeos_libc)
target_include_directories(os PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_compile_definitions(os PRIVATE -DOS_VERSION="${INCLUDEOS_VERSION}")

add_library(os_mockable STATIC
    version.cpp
    $<TARGET_OBJECTS:hal>
    $<TARGET_OBJECTS:kernel_mockable>
    $<TARGET_OBJECTS:util>
    $<TARGET_OBJECTS:net_mockable>
    $<TARGET_OBJECTS:fs>
    $<TARGET_OBJECTS:posix_mockable>
    $<TARGET_OBJECTS:virtio_mockable>
    $<TARGET_OBJECTS:hw>
    $<TARGET_OBJECTS:paging>
    )
target_link_libraries(os_mockable PRIVATE includeos_impl includeos_libcxx includeos_libc)
target_include_directories(os_mockable PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_compile_definitions(os_mockable PRIVATE -DOS_VERSION="${INCLUDEOS_VERSION}")

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
