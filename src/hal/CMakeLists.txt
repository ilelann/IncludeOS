set(SRCS
    machine.cpp
)

add_library(hal OBJECT ${SRCS})
target_link_libraries(hal PRIVATE
    includeos_impl
    includeos_libcxx
    includeos_libc
)
