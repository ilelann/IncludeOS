
set(SOURCES
    arch.cpp
    config.cpp
    main.cpp
    os.cpp
    profile.cpp
    drivers/memdisk.cpp
  )

#if (NOT PORTABLE)
#  set(SOURCES ${SOURCES} drivers/tap_driver.cpp linux_evloop.cpp)
#endif()

add_library(linuxrt STATIC ${SOURCES})
target_include_directories(linuxrt PRIVATE ../../src/include)
target_link_libraries(linuxrt PRIVATE
    arch
    includeos_api
    includeos_libcxx
    includeos_libc
)

install(TARGETS linuxrt DESTINATION $ENV{INCLUDEOS_PREFIX}/includeos/linux)
install(FILES drivers/usernet.hpp DESTINATION $ENV{INCLUDEOS_PREFIX}/includeos/linux/drivers)
