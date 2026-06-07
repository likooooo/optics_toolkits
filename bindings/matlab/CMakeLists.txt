find_package(Matlab QUIET COMPONENTS MX_LIBRARY)

if(NOT Matlab_FOUND)
    message(STATUS "MATLAB not found; skipping MEX bindings (set OPTICS_BUILD_MATLAB=OFF to silence)")
    return()
endif()

matlab_add_mex(
    NAME optics_tmm_mex
    SRC optics_tmm_mex.c
    LINK_TO optics_toolkits
    OUTPUT_NAME optics_tmm_mex
)

set(OPTICS_MATLAB_PACKAGE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/+optics)
install(DIRECTORY ${OPTICS_MATLAB_PACKAGE_DIR}/
    DESTINATION ${CMAKE_INSTALL_DATADIR}/optics_toolkits/matlab/+optics
    FILES_MATCHING PATTERN "*.m"
)

install(TARGETS optics_tmm_mex
    LIBRARY DESTINATION ${CMAKE_INSTALL_DATADIR}/optics_toolkits/matlab
    RUNTIME DESTINATION ${CMAKE_INSTALL_DATADIR}/optics_toolkits/matlab
)
