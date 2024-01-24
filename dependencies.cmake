
function (find_submodule name path)
    get_filename_component (submodules_base_dir ${CMAKE_CURRENT_LIST_DIR} DIRECTORY BASE_DIR)
    # message ("submodules_base_dir: ${submodules_base_dir}/${name}")
    if (EXISTS ${submodules_base_dir}/${name})
        set (${path} ${submodules_base_dir}/${name} PARENT_SCOPE)
    elseif (EXISTS ${3rdPARTY_DIR}/${name})
        set (${path} ${3rdPARTY_DIR}/${name} PARENT_SCOPE)
    endif()
endfunction()

function (add_psi_dependency parentName name)
    find_submodule (psi-${name} dep_path)
    message("psi_${name}_dir: ${dep_path}")
    if (EXISTS ${dep_path} AND NOT EXISTS ${dep_path}/build AND NOT ${parentName} STREQUAL "")
        message("configuring submodule [psi-${name}]... ${dep_path}")
        add_subdirectory(${dep_path} ${dep_path}/build)
    endif()
    set (${name}_found true)

    include_directories(${dep_path}/psi/include)
    link_directories(${dep_path}/build/bin/${CMAKE_BUILD_TYPE})

    if (${${name}_found})
        if (${name} STREQUAL "logger")
            message("found psi-logger")
            add_compile_definitions(PSI_LOGGER)
        endif()
    endif()
endfunction()

add_psi_dependency("" comm)
add_psi_dependency("" tools)
add_psi_dependency(psi-logger thread)
add_psi_dependency(psi-logger shared)
