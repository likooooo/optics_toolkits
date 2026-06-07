add_executable(test_tmm test_tmm.c)
target_link_libraries(test_tmm PRIVATE optics_toolkits)
add_test(NAME test_tmm_numeric COMMAND test_tmm)
set_tests_properties(test_tmm_numeric PROPERTIES LABELS "core")

add_executable(test_viz test_viz.c)
target_link_libraries(test_viz PRIVATE optics_toolkits)
add_test(NAME test_viz COMMAND test_viz)
set_tests_properties(test_viz PROPERTIES
    LABELS "viz"
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

if(NOT OPTICS_ENABLE_PYTHON_CORE_PLUGINS)
    set_tests_properties(test_viz PROPERTIES DISABLED TRUE)
endif()
