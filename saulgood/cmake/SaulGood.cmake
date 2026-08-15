if(NOT TARGET saulgood)
    find_program(SAULGOOD_EXE NAMES saulgood)
endif()

function(saulgood_add_tests)
    set(options "")
    set(one_value_keywords TARGET NAME OUTPUT)
    set(multi_value_keywords FILES LIBRARIES)
    cmake_parse_arguments(SG "${options}" "${one_value_keywords}" "${multi_value_keywords}" ${ARGN})

    if(NOT SG_FILES)
        message(FATAL_ERROR "saulgood_add_tests: FILES keyword is required.")
    endif()

    if(NOT SG_NAME)
        if(SG_TARGET)
            set(SG_NAME "${SG_TARGET}_tests")
        else()
            set(SG_NAME "saulgood_tests")
        endif()
    endif()

    if(NOT SG_OUTPUT)
        set(SG_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${SG_NAME}.c")
    endif()

    set(abs_files "")
    foreach(f IN LISTS SG_FILES)
        get_filename_component(abs_f "${f}" ABSOLUTE)
        list(APPEND abs_files "${abs_f}")
    endforeach()

    if(TARGET saulgood)
        set(EXEC_COMMAND $<TARGET_FILE:saulgood>)
        set(EXEC_DEPENDS saulgood ${abs_files})
    elseif(SAULGOOD_EXE)
        set(EXEC_COMMAND "${SAULGOOD_EXE}")
        set(EXEC_DEPENDS ${abs_files})
    else()
        message(FATAL_ERROR
            "saulgood_add_tests: neither the 'saulgood' target nor a system 'saulgood' executable was found.")
    endif()

    add_custom_command(
        OUTPUT "${SG_OUTPUT}"
        COMMAND ${EXEC_COMMAND} ${abs_files} -o "${SG_OUTPUT}"
        DEPENDS ${EXEC_DEPENDS}
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        COMMENT "Generating test runner: ${SG_NAME}.c"
        VERBATIM
    )

    add_library(${SG_NAME} SHARED "${SG_OUTPUT}")

    target_compile_definitions(${SG_NAME} 
        PRIVATE 
            DEBUG 
            _DEBUG
        INTERFACE
            $<VERTEX_COMPILE_LANGUAGE:C,CXX>
    )
    
    if(MSVC)
        target_compile_options(${SG_NAME} PRIVATE /Od /Zi /U _NDEBUG /U NDEBUG)
    else()
        target_compile_options(${SG_NAME} PRIVATE -O0 -g -U NDEBUG)
    endif()

    set_target_properties(${SG_NAME} PROPERTIES 
        MAP_IMPORTED_CONFIG_RELEASE DEBUG
        MAP_IMPORTED_CONFIG_MINSIZEREL DEBUG
        MAP_IMPORTED_CONFIG_RELWITHDEBINFO DEBUG
    )

    if(SG_LIBRARIES)
        target_link_libraries(${SG_NAME} PRIVATE ${SG_LIBRARIES})
    endif()

    add_dependencies(${SG_NAME} ${SG_TARGET})

    if(SG_TARGET AND TARGET ${SG_TARGET})
        get_target_property(target_type ${SG_TARGET} TYPE)

        if(target_type MATCHES ".*_LIBRARY")
            target_link_libraries(${SG_NAME} PRIVATE ${SG_TARGET})
        elseif(target_type STREQUAL "EXECUTABLE")
            target_compile_definitions(
                ${SG_NAME}
                PRIVATE
                SG_TARGET_EXECUTABLE="$<TARGET_FILE:${SG_TARGET}>"
            )
        endif()
    endif()

endfunction()
