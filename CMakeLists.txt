# 3.18.0 for SOURCE_SUBDIR FetchContent
cmake_minimum_required(VERSION 3.18.0 FATAL_ERROR)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
#we are only creating libraries for ELF
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

#set(CMAKE_C_COMPILER_TARGET x86_64-elf-none)
#set(CMAKE_CXX_COMPILER_TARGET x86_64-elf-none)

set(CMAKE_CXX_EXTENSIONS NO)

project(includeos)

include(FetchContent)
include(CTest)


enable_language(C)
enable_language(CXX)

option(PROFILE "Compile with startup profilers" OFF)
option(FOR_PRODUCTION "Stop the OS when conditions not suitable for production" OFF)
option(INCLUDEOS_INFO "activate info diagnostics" OFF)
option(INCLUDEOS_DEBUG "activate debug diagnostics" OFF)
option(INCLUDEOS_URI_THROW_ON_ERROR "throws when invalid uri is passed" ON)

set(INCLUDEOS_VERSION v0.0.0.1 CACHE STRING "IncludeOS version")

set(ARCH ${CMAKE_HOST_SYSTEM_PROCESSOR} CACHE STRING "IncludeOS target architecture")
set(PLATFORM x86_pc CACHE STRING "IncludeOS target platform")

# basically all deps should use those flags to prevent linking against host libs
add_library(includeos_flags INTERFACE)
target_compile_options(includeos_flags INTERFACE -nostdinc++ -nostdlib -nodefaultlibs)
target_link_options(includeos_flags INTERFACE -nostdlib -nodefaultlibs)
if (NOT INCLUDEOS_INFO)
  target_compile_definitions(includeos_flags INTERFACE -DNO_INFO=1)
endif()
if (NOT INCLUDEOS_DEBUG)
  target_compile_definitions(includeos_flags INTERFACE -DNO_DEBUG=1)
endif()
if (INCLUDEOS_URI_THROW_ON_ERROR)
  target_compile_definitions(includeos_flags INTERFACE -DURI_THROW_ON_ERROR)
endif()
# arguable but some test rely on it currently
target_compile_definitions(includeos_flags INTERFACE GSL_THROW_ON_CONTRACT_VIOLATION)
# convenience, not strictly required
target_compile_features(includeos_flags INTERFACE cxx_std_20)
# TODO temp hack
target_compile_definitions(includeos_flags INTERFACE -DARCH_x86_64)

fetchcontent_declare(gsl
    GIT_REPOSITORY https://github.com/microsoft/GSL.git
    GIT_TAG v2.0.0
)

fetchcontent_declare(uzlib
    GIT_REPOSITORY https://github.com/pfalcon/uzlib.git
    GIT_TAG v2.1.1
)

fetchcontent_declare(http-parser
    GIT_REPOSITORY https://github.com/nodejs/http-parser
    GIT_TAG v2.8.1
)

set(RAPIDJSON_BUILD_EXAMPLES OFF)
set(RAPIDJSON_BUILD_TESTS OFF)
fetchcontent_declare(rapidjson
    GIT_REPOSITORY https://github.com/Tencent/rapidjson.git
)

set(LEST_BUILD_TEST OFF)
set(LEST_BUILD_EXAMPLE OFF)
fetchcontent_declare(lest
    GIT_REPOSITORY https://github.com/martinmoene/lest.git
)

fetchcontent_declare(botan
    GIT_REPOSITORY https://github.com/randombit/botan.git
)

fetchcontent_declare(musl
    GIT_REPOSITORY git://git.musl-libc.org/musl
    GIT_TAG v1.2.1
    # TODO move musl to ExternalProject_Add ?
    # can't find a way to configure & make with fetchcontent
    #CONFIGURE_COMMAND configure
    #BUILD_COMMAND make
    #INSTALL_COMMAND make install
)

# all those flags to basically tell to LLVM:
# "give me a standalone static libcxx, including libcxxabi and libunwind deps, and using musl as libc"
# required to avoid final executable to link to host libc, libpthread, etc

set(LIBUNWIND_ENABLE_STATIC ON)
set(LIBUNWIND_ENABLE_SHARED OFF)
set(LIBUNWIND_ENABLE_THREADS OFF)
#set(LIBUNWIND_ENABLE_WERROR ON)
set(LIBUNWIND_INCLUDE_DOCS OFF)

set(LIBCXXABI_ENABLE_STATIC ON)
set(LIBCXXABI_ENABLE_SHARED OFF)
#set(LIBCXXABI_ENABLE_WERROR ON)
set(LIBCXXABI_INCLUDE_TESTS OFF)
set(LIBCXXABI_ENABLE_THREADS OFF)

set(LIBCXXABI_USE_LLVM_UNWINDER ON)
set(LIBCXXABI_ENABLE_STATIC_UNWINDER ON)
set(LIBCXXABI_HAS_GCC_S_LIB OFF)

set(LIBCXX_ENABLE_STATIC ON)
set(LIBCXX_ENABLE_SHARED OFF)
set(LIBCXX_ENABLE_THREADS OFF)
set(LIBCXX_ENABLE_EXPERIMENTAL_LIBRARY ON)
set(LIBCXX_ENABLE_FILESYSTEM OFF)
#set(LIBCXX_ENABLE_WERROR ON)
set(LIBCXX_INCLUDE_BENCHMARKS OFF)

set(LIBCXX_HAS_MUSL_LIBC ON)

set(LIBCXX_HAS_ATOMIC_LIB OFF)
set(LIBCXX_HAS_GCC_LIB OFF)
set(LIBCXX_HAS_GCC_S_LIB OFF)
set(LIBCXX_HAS_M_LIB OFF)
set(LIBCXX_HAS_PTHREAD_LIB OFF)
set(LIBCXX_HAS_RT_LIB OFF)

set(LLVM_INCLUDE_BENCHMARKS OFF)
set(LLVM_INCLUDE_EXAMPLES OFF)
set(LLVM_INCLUDE_DOCS OFF)
# TODO needs an upstream patch to work but does not hurt
# upstream patch https://reviews.llvm.org/D90025
# already in https://github.com/ilelann/llvm-project.git, cf below
set(LLVM_INCLUDE_LIB OFF)
set(LLVM_INCLUDE_RUNTIMES OFF)
set(LLVM_INCLUDE_TESTS OFF)
set(LLVM_INCLUDE_TOOLS OFF)
set(LLVM_INCLUDE_UTILS OFF)

set(LLVM_ENABLE_LIBXML2 OFF CACHE INTERNAL "")
set(LLVM_ENABLE_BINDINGS OFF)
set(LLVM_ENABLE_PROJECTS "libunwind;libcxxabi;libcxx" CACHE INTERNAL "")

fetchcontent_declare(llvm
    #GIT_REPOSITORY https://github.com/llvm/llvm-project.git
    #GIT_TAG llvmorg-11.0.0

    # for LLVM_INCLUDE_LIB
    GIT_REPOSITORY https://github.com/ilelann/llvm-project.git
    SOURCE_SUBDIR llvm
)

FetchContent_Declare(openssl
    GIT_REPOSITORY https://github.com/openssl/openssl
)

# TODO restore liveupdate
#FetchContent_MakeAvailable(openssl)
#FetchContent_GetProperties(openssl SOURCE_DIR opensslSourceDir)
#add_library(libcrypto)
#file(GLOB libcrypto_SOURCES ${opensslSourceDir}/crypto)
#target_sources(libcrypto PRIVATE ${libcrypto_SOURCES})
#set_target_properties(libcrypto PROPERTIES LINKER_LANGUAGE C)
#set(LibCrypto_FOUND true)


# TODO restore liveupdate
#list(APPEND S2N_CMAKE_ARGS CMAKE_C_FLAGS="-c" CMAKE_CXX_FLAGS="-c" )
#FetchContent_Declare(s2n
#        #GIT_REPOSITORY https://github.com/awslabs/s2n.git
#        #GIT_TAG main
#        # forked to make it compile cleanly (sign-compare)
#        GIT_REPOSITORY https://github.com/ilelann/s2n.git
#        CMAKE_ARGS ${S2N_CMAKE_ARGS}
#        )
## for s2n
#find_package(OpenSSL REQUIRED)

fetchcontent_makeavailable(
    gsl
    uzlib
    http-parser
    rapidjson
    musl
    llvm
)

# TODO restore liveupdate
#FetchContent_MakeAvailable(s2n)
#FetchContent_MakeAvailable(botan)

#musl
add_library(musl_iface INTERFACE)
fetchcontent_getproperties(musl SOURCE_DIR muslSourceDir)
fetchcontent_getproperties(musl BINARY_DIR muslBinaryDir)
# TODO move musl to ExternalProject_Add ?
execute_process(COMMAND ${muslSourceDir}/configure --disable-shared WORKING_DIRECTORY ${muslBinaryDir})
execute_process(COMMAND make -j WORKING_DIRECTORY ${muslBinaryDir})
target_include_directories(musl_iface SYSTEM INTERFACE ${muslSourceDir}/include)
target_include_directories(musl_iface SYSTEM INTERFACE ${muslBinaryDir}/obj/include)
target_include_directories(musl_iface SYSTEM INTERFACE ${muslSourceDir}/arch/generic)
target_include_directories(musl_iface SYSTEM INTERFACE ${muslSourceDir}/arch/x86_64)
target_link_directories(musl_iface INTERFACE ${muslBinaryDir}/lib)
target_link_libraries(musl_iface INTERFACE c)


#llvm
target_link_libraries(unwind_static PRIVATE musl_iface)
target_link_libraries(unwind_static PRIVATE includeos_flags)

target_compile_options(cxxabi_static PRIVATE -nostdinc)
target_link_libraries(cxxabi_static PRIVATE musl_iface)
target_link_libraries(cxxabi_static PRIVATE includeos_flags)

target_compile_options(cxx_static PRIVATE -nostdinc)
target_link_libraries(cxx_static PRIVATE musl_iface)
target_link_libraries(cxx_static PRIVATE includeos_flags)
# TODO would probably be better fixed by specifying so include paths as SYSTEM
target_compile_options(cxx_static PUBLIC -Wno-user-defined-literals)

# c/c++ lib aliases
add_library(includeos_libc ALIAS musl_iface)
add_library(includeos_libcxx ALIAS cxx_static)
add_library(includeos_libcxxabi ALIAS cxxabi_static)


# uzlib
fetchcontent_getproperties(uzlib SOURCE_DIR uzlibSourceDir)
file(GLOB UZLIB_SOURCES ${uzlibSourceDir}/src/*.c)
add_library(uzlib STATIC ${UZLIB_SOURCES})
target_link_libraries(uzlib PRIVATE includeos_flags)
target_link_libraries(uzlib PRIVATE includeos_libc)
target_include_directories(uzlib PUBLIC ${uzlibSourceDir}/src)

#http-parser
fetchcontent_getproperties(http-parser SOURCE_DIR httpParserSourceDir)
add_library(http-parser STATIC ${httpParserSourceDir}/http_parser.c)
target_include_directories(http-parser PUBLIC ${httpParserSourceDir})
target_link_libraries(http-parser PRIVATE includeos_flags)
target_link_libraries(http-parser PRIVATE includeos_libc)

#rapidjson
fetchcontent_getproperties(rapidjson SOURCE_DIR rapidjsonSourceDir)
add_library(rapidjson INTERFACE)
target_include_directories(rapidjson INTERFACE ${rapidjsonSourceDir}/include)

#botan
#FetchContent_GetProperties(botan SOURCE_DIR botanSourceDir)
#add_library(botan)
#target_include_directories(botan PUBLIC ${botanSourceDir}/src)
#target_link_libraries(botan INTERFACE botan-2)

add_library(includeos_api INTERFACE)
target_include_directories(includeos_api INTERFACE api)
target_link_libraries(includeos_api INTERFACE includeos_flags)
target_link_libraries(includeos_api INTERFACE GSL rapidjson)

add_subdirectory(src)
# TODO restore liveupdate
#add_subdirectory(lib/LiveUpdate)

#if (BUILD_TESTING)
add_subdirectory(test)
add_subdirectory(userspace)
#endif ()

# Install cmake files
install(FILES cmake/linux.service.cmake DESTINATION cmake)
install(FILES cmake/os.cmake DESTINATION cmake)
install(FILES cmake/includeos.cmake DESTINATION cmake)

install(DIRECTORY api/ DESTINATION include/os)
