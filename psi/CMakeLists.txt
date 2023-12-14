
include_directories(
    ${psi_comm_dir}/psi/include
    ${psi_shared_dir}/psi/include
    ${psi_thread_dir}/psi/include
    ${psi_tools_dir}/psi/include
    include
    src
)

set(SOURCES
    src/psi/logger/LoggerProxy.cpp
    src/psi/logger/LoggerSingleton.cpp
    src/psi/logger/ThreadMap.cpp
)

link_directories(
    ${psi_shared_dir}/build/bin/${CMAKE_BUILD_TYPE}
)

add_library(psi-logger STATIC ${SOURCES})

# set(TEST_SRC
#     tests/.cpp
# )
# psi_make_tests("Logger" "${TEST_SRC}" "psi-logger")

set(EXAMPLE_SRC_1.0 examples/1_Simple_Logger/EntryPoint.cpp)
psi_make_examples("1_Simple_Logger" "${EXAMPLE_SRC_1.0}" "psi-logger;psi-shared")