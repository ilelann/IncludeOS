set(SRCS
    virtio_queue.cpp
    )

set(MOCKABLE_SRCS
    virtio.cpp
    )

add_library(virtio OBJECT ${SRCS} ${MOCKABLE_SRCS})
target_link_libraries(virtio PRIVATE
    includeos_impl
    includeos_libcxx
    includeos_libc
    )

add_library(virtio_mockable OBJECT ${SRCS})
target_link_libraries(virtio_mockable PRIVATE
    includeos_impl
    includeos_libcxx
    includeos_libc
    )
