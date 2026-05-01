set(SOURCES
    src/psi/logger/LoggerProxy.cpp
    src/psi/logger/LoggerSingleton.cpp
    src/psi/logger/ThreadMap.cpp
)

set(target_lib "psi-logger")

add_library(${target_lib} STATIC ${SOURCES})
add_library(psi::logger ALIAS ${target_lib})

target_include_directories(${target_lib}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

psi_config_target(${target_lib})

# set(TEST_SRC
#     tests/.cpp
# )
# psi_make_tests("Logger" "${TEST_SRC}" "psi-logger")

set(EXAMPLE_SRC_1.0 examples/1_Simple_Logger/EntryPoint.cpp)
psi_make_examples("1_Simple_Logger" "${EXAMPLE_SRC_1.0}" "psi-logger")