add_library(optics_toolkits_fortran STATIC optics_toolkits.f90)
target_link_libraries(optics_toolkits_fortran PUBLIC optics_toolkits)
set_target_properties(optics_toolkits_fortran PROPERTIES
    Fortran_MODULE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/mod"
    POSITION_INDEPENDENT_CODE ON
)
target_include_directories(optics_toolkits_fortran INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/mod>
)

if(OPTICS_BUILD_EXAMPLES)
    add_subdirectory(${CMAKE_SOURCE_DIR}/examples/fortran ${CMAKE_CURRENT_BINARY_DIR}/fortran_examples)
endif()
