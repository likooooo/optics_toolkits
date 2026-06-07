set(OPTICS_CSHARP_LIBRARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/OpticsToolkits)
set(OPTICS_CSHARP_EXAMPLES_DIR ${CMAKE_SOURCE_DIR}/examples/csharp)
set(OPTICS_CSHARP_EXAMPLES
    01_poynting_absorption
    02_reflection_spectrum
    03_transmission_spectrum
    04_ellipsometry
    05_surface_plasmon_resonance
)

add_custom_target(optics_toolkits_csharp_lib
    COMMAND ${CMAKE_COMMAND} -E env
        "LD_LIBRARY_PATH=${CMAKE_BINARY_DIR}:$ENV{LD_LIBRARY_PATH}"
        dotnet build ${OPTICS_CSHARP_LIBRARY_DIR}/OpticsToolkits.csproj -c Release --nologo
    DEPENDS optics_toolkits
    WORKING_DIRECTORY ${OPTICS_CSHARP_LIBRARY_DIR}
    COMMENT "Building OpticsToolkits C# bindings"
    VERBATIM
)

add_custom_target(optics_toolkits_csharp ALL DEPENDS optics_toolkits_csharp_lib)

foreach(name IN LISTS OPTICS_CSHARP_EXAMPLES)
    add_custom_target(optics_csharp_${name}
        COMMAND ${CMAKE_COMMAND} -E env
            "LD_LIBRARY_PATH=${CMAKE_BINARY_DIR}:$ENV{LD_LIBRARY_PATH}"
            dotnet build ${OPTICS_CSHARP_EXAMPLES_DIR}/${name}/${name}.csproj -c Release --nologo
        DEPENDS optics_toolkits_csharp_lib
        WORKING_DIRECTORY ${OPTICS_CSHARP_EXAMPLES_DIR}/${name}
        COMMENT "Building C# example ${name}"
        VERBATIM
    )
    add_dependencies(optics_toolkits_csharp optics_csharp_${name})
endforeach()

if(OPTICS_BUILD_TESTS)
    foreach(name IN LISTS OPTICS_CSHARP_EXAMPLES)
        add_test(
            NAME example_csharp_${name}
            COMMAND ${CMAKE_COMMAND} -E env
                "LD_LIBRARY_PATH=${CMAKE_BINARY_DIR}:$ENV{LD_LIBRARY_PATH}"
                dotnet run --project ${OPTICS_CSHARP_EXAMPLES_DIR}/${name}/${name}.csproj -c Release --no-build
            WORKING_DIRECTORY ${OPTICS_CSHARP_EXAMPLES_DIR}/${name}
        )
        set_tests_properties(example_csharp_${name} PROPERTIES LABELS "examples;smoke;csharp")
    endforeach()
endif()
