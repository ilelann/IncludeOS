set(SRCS
    fd.cpp
    )

set(MOCKABLE_SRCS
    file_fd.cpp
    tcp_fd.cpp
    udp_fd.cpp
    unix_fd.cpp
    )

add_library(posix OBJECT ${SRCS} ${MOCKABLE_SRCS})
target_link_libraries(posix PRIVATE
    includeos_impl
    includeos_libcxx
    includeos_libc
    )

add_library(posix_mockable OBJECT ${SRCS})
target_link_libraries(posix_mockable PRIVATE
    includeos_impl
    includeos_libcxx
    includeos_libc
    )
