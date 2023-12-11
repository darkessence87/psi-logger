set( CRASH_DUMP_DIR ${BUILD_DIR}/bin/${CMAKE_BUILD_TYPE}/crashdumps)
configure_file(${CMAKE_SOURCE_DIR}/app_config.h.in ${SRC_DIR}/psi/include/psi/app_config.h)
file(MAKE_DIRECTORY ${CRASH_DUMP_DIR})

include_directories(
    ${3rdPARTY_DIR}/psi/include
    include
    src
)

set(SOURCES
    src/psi/logger/LoggerProxy.cpp
    src/psi/logger/LoggerSingleton.cpp
    src/psi/logger/ThreadMap.cpp
)

link_directories(
    ${3rdPARTY_DIR}/psi/lib/${SUB_DIR_LIBS}
)

add_library(psi-logger STATIC ${SOURCES})
target_link_libraries(psi-logger
    psi-thread
    psi-shared
)

# set(TEST_SRC
#     tests/TimerTests.cpp
# )
# psi_make_tests("Shared" "${TEST_SRC}" "psi-shared")

set(EXAMPLE_SRC_1.0 examples/1_Simple_Logger/EntryPoint.cpp)
psi_make_examples("1_Simple_Logger" "${EXAMPLE_SRC_1.0}" "psi-logger")