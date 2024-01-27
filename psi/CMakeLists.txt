
include_directories(
    include
    src
)

set(SOURCES
    src/psi/logger/LoggerProxy.cpp
    src/psi/logger/LoggerSingleton.cpp
    src/psi/logger/ThreadMap.cpp
)

add_library(psi-logger STATIC ${SOURCES})

# set(TEST_SRC
#     tests/.cpp
# )
# psi_make_tests("Logger" "${TEST_SRC}" "psi-logger")

set(EXAMPLE_SRC_1.0 examples/1_Simple_Logger/EntryPoint.cpp)
psi_make_examples("1_Simple_Logger" "${EXAMPLE_SRC_1.0}" "psi-logger")