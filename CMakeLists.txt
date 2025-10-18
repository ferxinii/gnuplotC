cmake_minimum_required(VERSION 3.20)
project(gnuplotC LANGUAGES C)

add_library(gnuplotC STATIC
    gnuplotc.c
)

target_include_directories(gnuplotC
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_options(gnuplotC
    PRIVATE
        -Wall -Werror -Wextra -O3
)

# Optional OMP support
option(GNUPLOTC_USE_OMP "Enable OMP support for parallel processing of frames. Default is by forking processes." OFF)
if(GNUPLOTC_USE_OMP)
    message(STATUS "Building gnuplotC with OpenMP support")
    find_package(OpenMP REQUIRED)
    target_compile_definitions(gnuplotC PRIVATE GNUPLOTC_USE_OMP)
    target_link_libraries(gnuplotC PRIVATE OpenMP::OpenMP_C)
endif()
