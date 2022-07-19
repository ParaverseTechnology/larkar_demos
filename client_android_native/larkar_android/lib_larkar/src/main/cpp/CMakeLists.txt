cmake_minimum_required(VERSION 3.4.1)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

message(STATUS "=========build lib larkar")

# huawei_arengine_ndk
add_library(huawei_arengine_ndk SHARED IMPORTED)
set_target_properties(huawei_arengine_ndk PROPERTIES IMPORTED_LOCATION
        "${ARENGINE_LIBPATH}/${ANDROID_ABI}/libhuawei_arengine_ndk.so")

# arcore
add_library(arcore SHARED IMPORTED)
set_target_properties(arcore PROPERTIES IMPORTED_LOCATION
        ${ARENGINE_LIBPATH}/${ANDROID_ABI}/libarcore_sdk_c.so)

# oboe
# Import Oboe
add_library(oboe-lib SHARED IMPORTED)
set_target_properties(oboe-lib PROPERTIES IMPORTED_LOCATION
        ${OBOE_LIBPATH}/${ANDROID_ABI}/liboboe.so
        )
include_directories(${OBOE_INCLUDE})

# larkxr sdk
add_library(lib_lark_xr SHARED IMPORTED)
set_target_properties(lib_lark_xr PROPERTIES IMPORTED_LOCATION
        ${ARENGINE_LIBPATH}/${ANDROID_ABI}/liblark_xr.so)

add_library(lib_lark_common SHARED IMPORTED)
set_target_properties(lib_lark_common PROPERTIES IMPORTED_LOCATION
        ${ARENGINE_LIBPATH}/${ANDROID_ABI}/liblark_common.so)

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
        ar_application.cpp
        jni_interface.cc
        util.cpp
        # huaweiengine
        hw_arengine/arengine_manager.cpp
        hw_arengine/arengine_background_render.cpp
        hw_arengine/arengine_plane_render.cpp
        # arcore
        arcore/arcore_manager.cpp
        arcore/arcore_plane_render.cpp
        arcore/arcore_background_render.cpp
        arcore/arcore_texture.cpp
)

if (ENABLE_CLOUDXR STREQUAL "1")
    list(APPEND src_files cloudxr_client.h)
    list(APPEND src_files cloudxr_client.cpp)
endif()

add_definitions(-D_GLM_ENABLE_EXPERIMENTAL)

add_library( # Sets the name of the library.
        lib_larkar
        # Sets the library as a shared library.
        SHARED
        # Provides a relative path to your source file(s).
        ${src_files}
)

set_target_properties(lib_larkar
        PROPERTIES
        OUTPUT_NAME larkar
)

# add include
target_include_directories(lib_larkar PRIVATE
        # lib xr
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${ARENGINE_LIBPATH}/include/
        ${GLM_LIBPATH}
)

find_library( # Sets the name of the path variable.
        log-lib
        log )

target_link_libraries( # Specifies the target library.
        lib_larkar
        android
        jnigraphics
        log
        # clang 注意顺序
        # gl render
        EGL
        GLESv3
        mediandk
        OpenMAXAL
        # arcore
        arcore
        # hwarengine
        huawei_arengine_ndk
        # pxy larkxr sdk
        lib_lark_xr
        lib_lark_common
        #oboe
        oboe-lib
)

if (ENABLE_CLOUDXR STREQUAL "1")
    target_link_libraries( # Specifies the target library.
            lib_larkar
            cloudxr-lib
    )
endif()