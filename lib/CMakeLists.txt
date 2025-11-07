add_library(legionaura_lib
    legionaura.cpp
)

target_include_directories(legionaura_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBUSB REQUIRED libusb-1.0)

target_link_libraries(legionaura_lib PUBLIC ${LIBUSB_LIBRARIES})
target_include_directories(legionaura_lib PUBLIC ${LIBUSB_INCLUDE_DIRS})
