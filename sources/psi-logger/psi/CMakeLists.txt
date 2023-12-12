
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
    psi-shared
    psi-thread
    psi-tools
)

# set(TEST_SRC
#     tests/.cpp
# )
# psi_make_tests("Logger" "${TEST_SRC}" "psi-logger")

set(EXAMPLE_SRC_1.0 examples/1_Simple_Logger/EntryPoint.cpp)
psi_make_examples("1_Simple_Logger" "${EXAMPLE_SRC_1.0}" "psi-logger")