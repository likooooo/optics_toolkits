set(OPTICS_C_EXAMPLES
    01_poynting_absorption
    02_reflection_spectrum
    03_transmission_spectrum
    04_ellipsometry
    05_surface_plasmon_resonance
)

foreach(name IN LISTS OPTICS_C_EXAMPLES)
    add_executable(${name} ${name}.c)
    target_include_directories(${name} PRIVATE ${CMAKE_SOURCE_DIR}/examples/common)
    target_link_libraries(${name} PRIVATE optics_toolkits)
endforeach()

if(OPTICS_BUILD_TESTS)
    foreach(name IN LISTS OPTICS_C_EXAMPLES)
        add_test(NAME example_${name} COMMAND ${name})
        set_tests_properties(example_${name} PROPERTIES LABELS "examples;smoke")
    endforeach()
endif()
