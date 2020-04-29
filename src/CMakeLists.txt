
project(input-device-interface)

set(TARGET_NAME input-device-interface)

find_package(PkgConfig)
pkg_search_module(LIBINPUT REQUIRED  libinput)
pkg_search_module(LIBUDEV REQUIRED libudev)

include_directories(${LIBINPUT_INCLUDE_DIRS})
include_directories(${LIBUDEV_INCLUDE_DIRS})

set(LIBRARY_OUTPUT_PATH ../lib)

add_library(${TARGET_NAME} SHARED  input_device_interface.h input_device_interface.c shared.h shared.c)

target_link_libraries(${TARGET_NAME} ${LIBINPUT_LIBRARIES} ${LIBUDEV_LIBRARIES})
target_compile_options(${TARGET_NAME} PUBLIC ${LIBINPUT_CFLAGS_OTHER})
target_compile_options(${TARGET_NAME} PUBLIC ${LIBUDEV_CFLAGS_OTHER})



