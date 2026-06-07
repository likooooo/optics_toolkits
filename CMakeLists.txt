cmake_minimum_required(VERSION 3.30)
project(optics_toolkits
    VERSION 0.1.0
    DESCRIPTION "Cross-language C ABI for simulation TMM/filmstack"
    HOMEPAGE_URL "https://github.com/likooooo/optics_toolkits"
    LANGUAGES CXX C Fortran
)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

include(cmake/precision.cmake)

option(OPTICS_BUILD_EXAMPLES "Build C examples" ON)
option(OPTICS_BUILD_TESTS "Build C ABI tests" ON)
option(OPTICS_BUILD_CSHARP "Build C# bindings and examples" ON)
option(OPTICS_BUILD_MATLAB "Build MATLAB MEX bindings" OFF)
option(OPTICS_BUILD_FORTRAN "Build Fortran bindings and examples" ON)

find_package(type_traist_notebook REQUIRED)

add_library(optics_toolkits SHARED
    src/c/optics_api.cpp
    src/c/optics_tmm.cpp
    src/c/optics_viz.cpp
)

target_compile_definitions(optics_toolkits PRIVATE optics_toolkits_EXPORTS)
target_include_directories(optics_toolkits
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/core/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src/c
)

include(cmake/python_plugins.cmake)

install(TARGETS optics_toolkits
    EXPORT optics_toolkits-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY include/optics_toolkits
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h"
)

configure_package_config_file(
    cmake/optics_toolkits-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/optics_toolkits-config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/optics_toolkits
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/optics_toolkits-config-version.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/optics_toolkits-config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/optics_toolkits-config-version.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/optics_toolkits
)

install(EXPORT optics_toolkits-targets
    FILE optics_toolkits-targets.cmake
    NAMESPACE optics_toolkits::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/optics_toolkits
)

if(OPTICS_BUILD_TESTS)
    enable_testing()
endif()

if(OPTICS_BUILD_EXAMPLES)
    add_subdirectory(examples/c)
endif()

if(OPTICS_BUILD_TESTS)
    add_subdirectory(tests/c)
endif()

if(OPTICS_BUILD_CSHARP)
    add_subdirectory(bindings/csharp)
endif()

if(OPTICS_BUILD_FORTRAN)
    add_subdirectory(bindings/fortran)
endif()

if(OPTICS_BUILD_MATLAB)
    add_subdirectory(bindings/matlab)
endif()

configure_file(cmake/optics_toolkits.pc.in ${CMAKE_CURRENT_BINARY_DIR}/optics_toolkits.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/optics_toolkits.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
