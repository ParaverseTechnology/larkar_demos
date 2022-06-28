cmake_minimum_required(VERSION 3.4.1)

# C++ flags.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
add_compile_options(-Wall -Wextra)

# Standard Android dependencies
find_library(android-lib android)
find_library(GLESv2-lib GLESv2)
find_library(GLESv3-lib GLESv3)
find_library(log-lib log)

message(STATUS "=========build lib larkvr")

# cardboard
add_library(cardboard SHARED IMPORTED)
set_target_properties(cardboard PROPERTIES IMPORTED_LOCATION
        ${LIBPATH}/${ANDROID_ABI}/libGfxPluginCardboard.so)

# larkxr sdk
add_library(lib_lark_xr SHARED IMPORTED)
set_target_properties(lib_lark_xr PROPERTIES IMPORTED_LOCATION
        ${LIBPATH}/${ANDROID_ABI}/liblark_xr.so)

add_library(lib_lark_common SHARED IMPORTED)
set_target_properties(lib_lark_common PROPERTIES IMPORTED_LOCATION
        ${LIBPATH}/${ANDROID_ABI}/liblark_common.so)

if (ENABLE_CLOUDXR STREQUAL "1")
    message(STATUS "=========start cloudxr ${CLOUDXR_LIBPATH}")
    # Import CloudXR
    add_library(cloudxr-lib SHARED IMPORTED)
    set_target_properties(cloudxr-lib PROPERTIES IMPORTED_LOCATION
            ${CLOUDXR_LIBPATH}/${ANDROID_ABI}/libCloudXRClient.so
            )
    include_directories(${CLOUDXR_INCLUDE})

    add_definitions(-DENABLE_CLOUDXR)
endif()

# source files
set(src_files
        util.h
        util.cc
        hello_cardboard_jni.cc
        hello_cardboard_app.h
        hello_cardboard_app.cc
        cloudxr_client.h
        cloudxr_client.cpp
)

if (ENABLE_CLOUDXR STREQUAL "1")
#    list(APPEND src_files cloudxr_client.h)
#    list(APPEND src_files cloudxr_client.cpp)
endif()

add_definitions(-D_GLM_ENABLE_EXPERIMENTAL)

add_library( # Sets the name of the library.
        lib_larkvr
        # Sets the library as a shared library.
        SHARED
        # Provides a relative path to your source file(s).
        ${src_files}
)

set_target_properties(lib_larkvr
        PROPERTIES
        OUTPUT_NAME larkvr
)

# add include
target_include_directories(lib_larkvr PRIVATE
        # lib xr
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${LIBPATH}/include/
        ${GLM_LIBPATH}
)

find_library( # Sets the name of the path variable.
        log-lib
        log )

target_link_libraries( # Specifies the target library.
        lib_larkvr
        jnigraphics
        ${android-lib}
        ${GLESv2-lib}
        ${GLESv3-lib}
        ${log-lib}
        # clang 注意顺序
        EGL
        # cardboard
        cardboard
        # pxy larkxr sdk
        lib_lark_xr
        lib_lark_common
)

if (ENABLE_CLOUDXR STREQUAL "1")
    target_link_libraries( # Specifies the target library.
            lib_larkvr
            cloudxr-lib
    )
endif()