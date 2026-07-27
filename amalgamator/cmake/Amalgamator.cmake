if(NOT TARGET amalgamator)
    find_program(AMALGAMATOR_EXE NAMES amalgamator)
endif()

function(target_amalgamation TARGET_NAME)
    set(options "")
    set(one_value_keywords NAME INPUT OUTPUT)
    set(multi_value_keywords FLAGS)

    cmake_parse_arguments(AMAL "${options}" "${one_value_keywords}" "${multi_value_keywords}" ${ARGN})

    if(NOT AMAL_NAME OR NOT AMAL_INPUT OR NOT AMAL_OUTPUT)
        message(FATAL_ERROR
            "target_amalgamation: NAME, INPUT, and OUTPUT are required keywords.")
    endif()

    get_filename_component(abs_input "${AMAL_INPUT}" ABSOLUTE)
    get_filename_component(abs_output "${AMAL_OUTPUT}" ABSOLUTE)

    if(TARGET amalgamator)
        set(EXEC_COMMAND $<TARGET_FILE:amalgamator>)
        set(EXEC_DEPENDS amalgamator "${abs_input}")
    elseif(AMALGAMATOR_EXE)
        set(EXEC_COMMAND "${AMALGAMATOR_EXE}")
        set(EXEC_DEPENDS "${abs_input}")
    else()
        message(FATAL_ERROR
            "target_amalgamation: neither the 'amalgamator' target nor a system 'amalgamator' executable was found.")
    endif()

    add_custom_command(
        OUTPUT "${abs_output}"
        COMMAND
            ${EXEC_COMMAND}
            "${abs_input}"
            ${AMAL_FLAGS}
            -o "${abs_output}"
        DEPENDS ${EXEC_DEPENDS}
        COMMENT "Amalgamating ${AMAL_NAME} -> ${AMAL_OUTPUT}"
        VERBATIM
    )

    add_custom_target(${AMAL_NAME}_amalgamation
        DEPENDS "${abs_output}"
    )

    add_dependencies(${TARGET_NAME} ${AMAL_NAME}_amalgamation)
endfunction()
