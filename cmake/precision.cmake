set(OPTICS_PRECISION "f64" CACHE STRING "Scalar precision: f32 or f64")

if(OPTICS_PRECISION STREQUAL "f32")
    add_compile_definitions(OPTICS_REAL_TYPE=float OPTICS_TOOLKITS_F32=1)
    set(OPTICS_PC_CFLAGS "-DOPTICS_TOOLKITS_F32=1")
elseif(OPTICS_PRECISION STREQUAL "f64")
    add_compile_definitions(OPTICS_REAL_TYPE=double OPTICS_TOOLKITS_F64=1)
    set(OPTICS_PC_CFLAGS "-DOPTICS_TOOLKITS_F64=1")
else()
    message(FATAL_ERROR "OPTICS_PRECISION must be f32 or f64, got: ${OPTICS_PRECISION}")
endif()

message(STATUS "optics_toolkits precision: ${OPTICS_PRECISION}")
