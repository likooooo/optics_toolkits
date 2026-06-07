set(OPTICS_FORTRAN_EXAMPLES
    01_poynting_absorption
    02_reflection_spectrum
    03_transmission_spectrum
    04_ellipsometry
    05_surface_plasmon_resonance
)

add_library(optics_fortran_examples_common STATIC csv_output.f90)
target_link_libraries(optics_fortran_examples_common PUBLIC optics_toolkits_fortran)

foreach(name IN LISTS OPTICS_FORTRAN_EXAMPLES)
    set(target fortran_${name})
    add_executable(${target} ${name}.f90)
    target_link_libraries(${target} PRIVATE optics_fortran_examples_common optics_toolkits)
    target_include_directories(${target} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        $<TARGET_PROPERTY:optics_toolkits_fortran,Fortran_MODULE_DIRECTORY>
    )
    set_target_properties(${target} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
endforeach()

if(OPTICS_BUILD_TESTS)
    foreach(name IN LISTS OPTICS_FORTRAN_EXAMPLES)
        add_test(NAME example_fortran_${name} COMMAND fortran_${name})
        set_tests_properties(example_fortran_${name} PROPERTIES
            LABELS "examples;smoke;fortran"
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        )
    endforeach()
endif()
