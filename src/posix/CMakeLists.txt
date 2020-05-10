SET(SRCS
      fd.cpp

    )
if (NOT CMAKE_TESTING_ENABLED)
  list(APPEND SRCS
    file_fd.cpp
    tcp_fd.cpp
    udp_fd.cpp
    unix_fd.cpp
  )
endif()
add_library(posix OBJECT ${SRCS})
target_link_libraries(posix PRIVATE
    includeos_impl
    includeos_libcxx
    includeos_libc
)
