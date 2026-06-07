option(OPTICS_ENABLE_PYTHON_CORE_PLUGINS "Enable py_helper/core_plugins via C ABI" OFF)

if(OPTICS_ENABLE_PYTHON_CORE_PLUGINS)
    find_package(py_visualizer REQUIRED)
    find_package(Boost COMPONENTS python numpy REQUIRED)
    find_package(Python3 REQUIRED COMPONENTS Development)

    target_compile_definitions(optics_toolkits PRIVATE OPTICS_HAS_PYTHON_CORE_PLUGINS=1)
    target_link_libraries(optics_toolkits PRIVATE py_visualizer::py_visualizer)
    copy_visualizer_files(optics_toolkits ${CMAKE_BINARY_DIR})
endif()
