if(NOT TARGET macromancer)
    find_program(MACROMANCER_EXE NAMES macromancer)
endif()

function(macromancer_generate)
    set(options "")
    set(one_value_keywords TARGET NAME INPUT OUTPUT VISIBILITY)
    set(multi_value_keywords FLAGS)

    cmake_parse_arguments(MM "${options}" "${one_value_keywords}" "${multi_value_keywords}" ${ARGN})

    if(NOT MM_INPUT)
        message(FATAL_ERROR "macromancer_generate: INPUT keyword is required.")
    endif()

    if(NOT MM_TARGET)
        message(FATAL_ERROR "macromancer_generate: TARGET keyword is required.")
    endif()

    if(NOT TARGET ${MM_TARGET})
        message(FATAL_ERROR "macromancer_generate: target '${MM_TARGET}' does not exist.")
    endif()

    # Default visibility to PRIVATE if not specified
    if(NOT MM_VISIBILITY)
        set(MM_VISIBILITY PRIVATE)
    endif()

    if(NOT MM_NAME)
        # Note: NAME_WLE requires CMake 3.14 or newer
        get_filename_component(MM_NAME "${MM_INPUT}" NAME_WLE)
    endif()

    if(NOT MM_OUTPUT)
        set(MM_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${MM_NAME}.h")
    endif()

    get_filename_component(abs_input "${MM_INPUT}" ABSOLUTE)
    get_filename_component(abs_output "${MM_OUTPUT}" ABSOLUTE)

    if(TARGET macromancer)
        set(EXEC_COMMAND $<TARGET_FILE:macromancer>)
        set(EXEC_DEPENDS macromancer "${abs_input}")
    elseif(MACROMANCER_EXE)
        set(EXEC_COMMAND "${MACROMANCER_EXE}")
        set(EXEC_DEPENDS "${abs_input}")
    else()
        message(FATAL_ERROR
            "macromancer_generate: neither the 'macromancer' target nor a system 'macromancer' executable was found.")
    endif()

    add_custom_command(
        OUTPUT "${abs_output}"
        COMMAND
            ${EXEC_COMMAND}
            "${abs_input}"
            ${MM_FLAGS}
            -o "${abs_output}"
        DEPENDS ${EXEC_DEPENDS}
        COMMENT "Generating ${MM_OUTPUT}"
        VERBATIM
    )

    target_sources(${MM_TARGET} ${MM_VISIBILITY} "${abs_output}")
    target_include_directories(${MM_TARGET} ${MM_VISIBILITY} "${CMAKE_CURRENT_BINARY_DIR}")
endfunction()
