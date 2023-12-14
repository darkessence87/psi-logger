
function (find_submodule name path)
    get_filename_component (submodules_base_dir ${CMAKE_CURRENT_LIST_DIR} DIRECTORY BASE_DIR)
    # message ("submodules_base_dir: ${submodules_base_dir}/${name}")
    if (EXISTS ${submodules_base_dir}/${name})
        set (${path} ${submodules_base_dir}/${name} PARENT_SCOPE)
    elseif (EXISTS ${3rdPARTY_DIR}/${name})
        set (${path} ${3rdPARTY_DIR}/${name} PARENT_SCOPE)
    endif()
endfunction()

function (add_psi_dependency name)
    if (NOT EXISTS psi_${name}_dir)
        find_submodule (psi-${name} dep_path)
        set (psi_${name}_dir ${dep_path} PARENT_SCOPE)
        message("psi_${name}_dir: ${dep_path}")
        if (NOT EXISTS ${dep_path}/build/bin/${CMAKE_BUILD_TYPE})
            if (EXISTS ${dep_path})
                message("configuring submodule [psi-${name}]... ${dep_path}")
                add_subdirectory(${dep_path})
                set (${name}_found true)
            endif()
        else()
            set (${name}_found true)
        endif()
    else()
        set (${name}_found true)
    endif()
endfunction()

add_psi_dependency(comm)
add_psi_dependency(thread)
add_psi_dependency(tools)
add_psi_dependency(shared)
